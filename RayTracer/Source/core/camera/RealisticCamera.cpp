#include "RealisticCamera.h"

#include "../bxdf/BxDF.h"
#include "../film/Film.h"
#include "../interaction/Interaction.h"
#include "../sampler/Sampling.h"
#include "../../common/math/Constants.h"
#include "../../common/math/Ray.h"
#include "../../common/math/RayDifferential.h"
#include "../../common/math/Transform.h"
#include "../../common/tool/MultiThread.h"
#include <array>

namespace core
{
namespace camera
{


RealisticCamera::RealisticCamera(const common::math::AnimatedTransformf &CameraToWorld,
    Float shutterOpen, Float shutterClose,
    Float apertureDiameter, Float focusDistance,
    bool simpleWeighting,
    std::vector<Float> &lensData, core::film::Film *film,
    const core::interaction::Medium *medium)
    : Camera(CameraToWorld, shutterOpen, shutterClose, film, medium),
    simpleWeighting(simpleWeighting)
{
    for (int i = 0; i < lensData.size(); i += 4)
    {
        if (FLOAT_0 == lensData[i])
        {
            if (apertureDiameter > lensData[i + 3])
            {
                /* TODO
                Warning(
                    "Specified aperture diameter %f is greater than maximum "
                    "possible %f.  Clamping it.",
                    apertureDiameter, lensData[i + 3]);
                */
            }
            else
            {
                lensData[i + 3] = apertureDiameter;
            }
        }
        elementInterfaces.push_back(LensElementInterface(
            {lensData[i] *  static_cast<Float>(0.001F), lensData[i + 1] * static_cast<Float>(0.001F),
            lensData[i + 2], lensData[i + 3] * static_cast<Float>(0.001F) / FLOAT_2}));
    }

    // Compute lens--film distance for given focus distance
    Float fb = FocusBinarySearch(focusDistance);
    /* TODO
    LOG(INFO) << StringPrintf("Binary search focus: %f -> %f\n", fb,
        FocusDistance(fb));
    */
    elementInterfaces.back().thickness = FocusThickLens(focusDistance);
    /* TODO
    LOG(INFO) << StringPrintf("Thick lens focus: %f -> %f\n",
        elementInterfaces.back().thickness,
        FocusDistance(elementInterfaces.back().thickness));
    */

    // Compute exit pupil bounds at sampled points on the film
    int nSamples = 64;
    exitPupilBounds.resize(nSamples);
    common::tool::ParallelFor([&](int i)
    {
        Float r0 = static_cast<Float>(i) / nSamples * film->diagonal / FLOAT_2;
        Float r1 = static_cast<Float>(i + 1) / nSamples * film->diagonal / FLOAT_2;
        exitPupilBounds[i] = BoundExitPupil(r0, r1);
    }, nSamples);

    if (simpleWeighting)
    {
        /* TODO
        Warning("\"simpleweighting\" option with RealisticCamera no longer "
            "necessarily matches regular camera images. Further, pixel "
            "values will vary a bit depending on the aperture size. See "
            "this discussion for details: "
            "https://github.com/mmp/pbrt-v3/issues/162#issuecomment-348625837");
        */
    }
}

bool RealisticCamera::TraceLensesFromFilm(const common::math::Rayf &rCamera, common::math::Rayf *rOut) const
{
    Float elementZ = FLOAT_0;
    // common::math::Transformf _rCamera_ from camera to lens system space
    static const common::math::Transformf CameraToLens = common::math::Scale(common::math::Vec3f(FLOAT_1, FLOAT_1, -FLOAT_1));
    common::math::Rayf rLens = CameraToLens(rCamera);
    for (int i = elementInterfaces.size() - 1; i >= 0; --i)
    {
        const LensElementInterface &element = elementInterfaces[i];
        // Update ray from film accounting for interaction with _element_
        elementZ -= element.thickness;

        // Compute intersection of ray with lens element
        Float t;
        common::math::Vec3f n;
        bool isStop = (FLOAT_0 == element.curvatureRadius);
        if (isStop)
        {
            // The refracted ray computed in the previous lens element
            // interface may be pointed towards film plane(+z) in some
            // extreme situations; in such cases, 't' becomes negative.
            if (rLens.dir.z >= FLOAT_0)
            {
                return false;
            }
            t = (elementZ - rLens.origin.z) / rLens.dir.z;
        }
        else
        {
            Float radius = element.curvatureRadius;
            Float zCenter = elementZ + element.curvatureRadius;
            if (!IntersectSphericalElement(radius, zCenter, rLens, &t, &n))
            {
                return false;
            }
        }
        CHECK_GE(t, FLOAT_0);

        // Test intersection point against element aperture
        common::math::Vec3f pHit = rLens(t);
        Float r2 = pHit.x * pHit.x + pHit.y * pHit.y;
        if (r2 > element.apertureRadius * element.apertureRadius) return false;
        rLens.origin = pHit;

        // Update ray path for element interface interaction
        if (!isStop)
        {
            common::math::Vec3f w;
            Float etaI = element.eta;
            Float etaT = (i > 0 && elementInterfaces[i - 1].eta != FLOAT_0)
                ? elementInterfaces[i - 1].eta : FLOAT_1;
            if (!core::bxdf::Refract(Normalize(-rLens.dir), n, etaI / etaT, &w))
            {
                return false;
            }
            rLens.dir = w;
        }
    }
    // common::math::Transformf _rLens_ from lens system space back to camera space
    if (rOut != nullptr)
    {
        static const common::math::Transformf LensToCamera = common::math::Scale(common::math::Vec3f(FLOAT_1, FLOAT_1, -FLOAT_1));
        *rOut = LensToCamera(rLens);
    }
    return true;
}

bool RealisticCamera::IntersectSphericalElement(Float radius, Float zCenter,
    const common::math::Rayf &ray, Float *t,
    common::math::Vec3f *n)
{
    // Compute _t0_ and _t1_ for ray--element intersection
    common::math::Vec3f o = ray.origin - common::math::Vec3f(FLOAT_0, FLOAT_0, zCenter);
    Float A = ray.dir.x * ray.dir.x + ray.dir.y * ray.dir.y + ray.dir.z * ray.dir.z;
    Float B = FLOAT_2 * (ray.dir.x * o.x + ray.dir.y * o.y + ray.dir.z * o.z);
    Float C = o.x * o.x + o.y * o.y + o.z * o.z - radius * radius;
    Float t0, t1;
    if (!common::math::Quadratic(A, B, C, &t0, &t1))
    {
        return false;
    }

    // Select intersection $t$ based on ray direction and element curvature
    bool useCloserT = (ray.dir.z > FLOAT_0) ^ (radius < FLOAT_0);
    *t = useCloserT ? (std::min)(t0, t1) : (std::max)(t0, t1);
    if (*t < FLOAT_0)
    {
        return false;
    }

    // Compute surface normal of element at ray intersection point
    *n = common::math::Vec3f(common::math::Vec3f(o + *t * ray.dir));
    *n = Faceforward(Normalize(*n), -ray.dir);
    return true;
}

bool RealisticCamera::TraceLensesFromScene(const common::math::Rayf &rCamera,
    common::math::Rayf *rOut) const
{
    Float elementZ = -LensFrontZ();
    // common::math::Transformf _rCamera_ from camera to lens system space
    static const common::math::Transformf CameraToLens = common::math::Scale(
        common::math::Vec3f(FLOAT_1, FLOAT_1, -FLOAT_1));
    common::math::Rayf rLens = CameraToLens(rCamera);
    for (size_t i = 0; i < elementInterfaces.size(); ++i)
    {
        const LensElementInterface &element = elementInterfaces[i];
        // Compute intersection of ray with lens element
        Float t;
        common::math::Vec3f n;
        bool isStop = (FLOAT_0 == element.curvatureRadius);
        if (isStop)
        {
            t = (elementZ - rLens.origin.z) / rLens.dir.z;
        }
        else
        {
            Float radius = element.curvatureRadius;
            Float zCenter = elementZ + element.curvatureRadius;
            if (!IntersectSphericalElement(radius, zCenter, rLens, &t, &n))
            {
                return false;
            }
        }
        CHECK_GE(t, FLOAT_0);

        // Test intersection point against element aperture
        common::math::Vec3f pHit = rLens(t);
        Float r2 = pHit.x * pHit.x + pHit.y * pHit.y;
        if (r2 > element.apertureRadius * element.apertureRadius)
        {
            return false;
        }
        rLens.origin = pHit;

        // Update ray path for from-scene element interface interaction
        if (!isStop)
        {
            common::math::Vec3f wt;
            Float etaI = (0 == i || FLOAT_0 == elementInterfaces[i - 1].eta)
                ? FLOAT_1 : elementInterfaces[i - 1].eta;
            Float etaT = (elementInterfaces[i].eta != FLOAT_0) ? elementInterfaces[i].eta : FLOAT_1;
            if (!core::bxdf::Refract(Normalize(-rLens.dir), n, etaI / etaT, &wt))
            {
                return false;
            }
            rLens.dir = wt;
        }
        elementZ += element.thickness;
    }
    // common::math::Transformf _rLens_ from lens system space back to camera space
    if (rOut != nullptr)
    {
        static const common::math::Transformf LensToCamera = common::math::Scale(common::math::Vec3f(FLOAT_1, FLOAT_1, -FLOAT_1));
        *rOut = LensToCamera(rLens);
    }
    return true;
}

/* TODO
void RealisticCamera::DrawLensSystem() const
{
    Float sumz = -LensFrontZ();
    Float z = sumz;
    for (size_t i = 0; i < elementInterfaces.size(); ++i)
    {
        const LensElementInterface &element = elementInterfaces[i];
        Float r = element.curvatureRadius;
        if (FLOAT_0 == r)
        {
            // stop
            printf("{Thick, Line[{{%f, %f}, {%f, %f}}], ", z,
                element.apertureRadius, z, 2 * element.apertureRadius);
            printf("Line[{{%f, %f}, {%f, %f}}]}, ", z, -element.apertureRadius,
                z, -2 * element.apertureRadius);
        }
        else
        {
            Float theta = std::abs(std::asin(element.apertureRadius / r));
            if (r > FLOAT_0)
            {
                // convex as seen from front of lens
                Float t0 = common::math::PI - theta;
                Float t1 = common::math::PI + theta;
                printf("Circle[{%f, 0}, %f, {%f, %f}], ", z + r, r, t0, t1);
            }
            else
            {
                // concave as seen from front of lens
                Float t0 = -theta;
                Float t1 = theta;
                printf("Circle[{%f, 0}, %f, {%f, %f}], ", z + r, -r, t0, t1);
            }
            if (element.eta != 0 && element.eta != 1)
            {
                // connect top/bottom to next element
                CHECK_LT(i + 1, elementInterfaces.size());
                Float nextApertureRadius =
                    elementInterfaces[i + 1].apertureRadius;
                Float h = (std::max)(element.apertureRadius, nextApertureRadius);
                Float hlow = (std::min)(element.apertureRadius, nextApertureRadius);

                Float zp0, zp1;
                if (r > FLOAT_0)
                {
                    zp0 = z + element.curvatureRadius -
                        element.apertureRadius / std::tan(theta);
                }
                else
                {
                    zp0 = z + element.curvatureRadius +
                        element.apertureRadius / std::tan(theta);
                }

                Float nextCurvatureRadius = elementInterfaces[i + 1].curvatureRadius;
                Float nextTheta = std::abs(std::asin(nextApertureRadius / nextCurvatureRadius));
                if (nextCurvatureRadius > FLOAT_0)
                {
                    zp1 = z + element.thickness + nextCurvatureRadius -
                        nextApertureRadius / std::tan(nextTheta);
                }
                else
                {
                    zp1 = z + element.thickness + nextCurvatureRadius +
                        nextApertureRadius / std::tan(nextTheta);
                }

                // Connect tops
                printf("Line[{{%f, %f}, {%f, %f}}], ", zp0, h, zp1, h);
                printf("Line[{{%f, %f}, {%f, %f}}], ", zp0, -h, zp1, -h);

                // vertical lines when needed to close up the element profile
                if (element.apertureRadius < nextApertureRadius)
                {
                    printf("Line[{{%f, %f}, {%f, %f}}], ", zp0, h, zp0, hlow);
                    printf("Line[{{%f, %f}, {%f, %f}}], ", zp0, -h, zp0, -hlow);
                }
                else if (element.apertureRadius > nextApertureRadius)
                {
                    printf("Line[{{%f, %f}, {%f, %f}}], ", zp1, h, zp1, hlow);
                    printf("Line[{{%f, %f}, {%f, %f}}], ", zp1, -h, zp1, -hlow);
                }
            }
        }
        z += element.thickness;
    }

    // 24mm height for 35mm film
    printf("Line[{{0, -.012}, {0, .012}}], ");
    // optical axis
    printf("Line[{{0, 0}, {%f, 0}}] ", 1.2f * sumz);
}

void RealisticCamera::DrawRayPathFromFilm(const common::math::Rayf &r, bool arrow,
    bool toOpticalIntercept) const
{
    Float elementZ = FLOAT_0;
    // common::math::Transformf _ray_ from camera to lens system space
    static const common::math::Transformf CameraToLens = common::math::Scale(common::math::Vec3f(FLOAT_1, FLOAT_1, -FLOAT_1));
    common::math::Rayf ray = CameraToLens(r);
    printf("{ ");
    if (!TraceLensesFromFilm(r, nullptr))
    {
        printf("Dashed, ");
    }
    for (int i = elementInterfaces.size() - 1; i >= 0; --i)
    {
        const LensElementInterface &element = elementInterfaces[i];
        elementZ -= element.thickness;
        bool isStop = (element.curvatureRadius == 0);
        // Compute intersection of ray with lens element
        Float t;
        common::math::Vec3f n;
        if (isStop)
            t = -(ray.origin.z - elementZ) / ray.dir.z;
        else
        {
            Float radius = element.curvatureRadius;
            Float zCenter = elementZ + element.curvatureRadius;
            if (!IntersectSphericalElement(radius, zCenter, ray, &t, &n))
                goto done;
        }
        CHECK_GE(t, 0);

        printf("Line[{{%f, %f}, {%f, %f}}],", ray.origin.z, ray.origin.x, ray(t).z,
            ray(t).x);

        // Test intersection point against element aperture
        common::math::Vec3f pHit = ray(t);
        Float r2 = pHit.x * pHit.x + pHit.y * pHit.y;
        Float apertureRadius2 = element.apertureRadius * element.apertureRadius;
        if (r2 > apertureRadius2) goto done;
        ray.origin = pHit;

        // Update ray path for element interface interaction
        if (!isStop)
        {
            common::math::Vec3f wt;
            Float etaI = element.eta;
            Float etaT = (i > 0 && elementInterfaces[i - 1].eta != 0)
                ? elementInterfaces[i - 1].eta
                : 1;
            if (!Refract(Normalize(-ray.dir), n, etaI / etaT, &wt)) goto done;
            ray.dir = wt;
        }
    }

    ray.dir = Normalize(ray.dir);
    {
        Float ta = std::abs(elementZ / 4);
        if (toOpticalIntercept)
        {
            ta = -ray.origin.x / ray.dir.x;
            printf("Point[{%f, %f}], ", ray(ta).z, ray(ta).x);
        }
        printf("%s[{{%f, %f}, {%f, %f}}]", arrow ? "Arrow" : "Line", ray.origin.z,
            ray.origin.x, ray(ta).z, ray(ta).x);

        // overdraw the optical axis if needed...
        if (toOpticalIntercept)
            printf(", Line[{{%f, 0}, {%f, 0}}]", ray.origin.z, ray(ta).z * 1.05f);
    }

done:
    printf("}");
}

void RealisticCamera::DrawRayPathFromScene(const common::math::Rayf &r, bool arrow,
    bool toOpticalIntercept) const
{
    Float elementZ = LensFrontZ() * -1;

    // common::math::Transformf _ray_ from camera to lens system space
    static const common::math::Transformf CameraToLens = common::math::Scale(common::math::Vec3f(FLOAT_1, FLOAT_1, -FLOAT_1));
    common::math::Rayf ray = CameraToLens(r);
    for (size_t i = 0; i < elementInterfaces.size(); ++i)
    {
        const LensElementInterface &element = elementInterfaces[i];
        bool isStop = (element.curvatureRadius == 0);
        // Compute intersection of ray with lens element
        Float t;
        common::math::Vec3f n;
        if (isStop)
            t = -(ray.origin.z - elementZ) / ray.dir.z;
        else
        {
            Float radius = element.curvatureRadius;
            Float zCenter = elementZ + element.curvatureRadius;
            if (!IntersectSphericalElement(radius, zCenter, ray, &t, &n))
                return;
        }
        CHECK_GE(t, 0.f);

        printf("Line[{{%f, %f}, {%f, %f}}],", ray.origin.z, ray.origin.x, ray(t).z,
            ray(t).x);

        // Test intersection point against element aperture
        common::math::Vec3f pHit = ray(t);
        Float r2 = pHit.x * pHit.x + pHit.y * pHit.y;
        Float apertureRadius2 = element.apertureRadius * element.apertureRadius;
        if (r2 > apertureRadius2) return;
        ray.origin = pHit;

        // Update ray path for from-scene element interface interaction
        if (!isStop)
        {
            common::math::Vec3f wt;
            Float etaI = (i == 0 || elementInterfaces[i - 1].eta == 0.f)
                ? 1.f
                : elementInterfaces[i - 1].eta;
            Float etaT = (elementInterfaces[i].eta != 0.f)
                ? elementInterfaces[i].eta
                : 1.f;
            if (!Refract(Normalize(-ray.dir), n, etaI / etaT, &wt)) return;
            ray.dir = wt;
        }
        elementZ += element.thickness;
    }

    // go to the film plane by default
    {
        Float ta = -ray.origin.z / ray.dir.z;
        if (toOpticalIntercept)
        {
            ta = -ray.origin.x / ray.dir.x;
            printf("Point[{%f, %f}], ", ray(ta).z, ray(ta).x);
        }
        printf("%s[{{%f, %f}, {%f, %f}}]", arrow ? "Arrow" : "Line", ray.origin.z,
            ray.origin.x, ray(ta).z, ray(ta).x);
    }
}
*/

void RealisticCamera::ComputeCardinalPoints(const common::math::Rayf &rIn, const common::math::Rayf &rOut,
    Float *pz, Float *fz)
{
    Float tf = -rOut.origin.x / rOut.dir.x;
    *fz = -rOut(tf).z;
    Float tp = (rIn.origin.x - rOut.origin.x) / rOut.dir.x;
    *pz = -rOut(tp).z;
}

void RealisticCamera::ComputeThickLensApproximation(Float pz[2], Float fz[2]) const
{
    // Find height $x$ from optical axis for parallel rays
    Float x = static_cast<Float>(0.001F) * film->diagonal;

    // Compute cardinal points for film side of lens system
    common::math::Rayf rScene(common::math::Vec3f(x, FLOAT_0, LensFrontZ() + FLOAT_1)
        , common::math::Vec3f(FLOAT_0, FLOAT_0, -FLOAT_1));
    common::math::Rayf rFilm;

    CHECK(TraceLensesFromScene(rScene, &rFilm));
    /* TODO
        << "Unable to trace ray from scene to film for thick lens "
        "approximation. Is aperture stop extremely small?";
    */
    ComputeCardinalPoints(rScene, rFilm, &pz[0], &fz[0]);

    // Compute cardinal points for scene side of lens system
    rFilm = common::math::Rayf(common::math::Vec3f(x, FLOAT_0, LensRearZ() - FLOAT_1)
        , common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_1));
    CHECK(TraceLensesFromFilm(rFilm, &rScene));
    /* TODO
        << "Unable to trace ray from film to scene for thick lens "
        "approximation. Is aperture stop extremely small?";
    */
    ComputeCardinalPoints(rFilm, rScene, &pz[1], &fz[1]);
}

Float RealisticCamera::FocusThickLens(Float focusDistance)
{
    Float pz[2], fz[2];
    ComputeThickLensApproximation(pz, fz);
    /* TODO
    LOG(INFO) << StringPrintf("Cardinal points: p' = %f f' = %f, p = %f f = %f.\n",
        pz[0], fz[0], pz[1], fz[1]);
    LOG(INFO) << StringPrintf("Effective focal length %f\n", fz[0] - pz[0]);
    */
    // Compute translation of lens, _delta_, to focus at _focusDistance_
    Float f = fz[0] - pz[0];
    Float z = -focusDistance;
    Float c = (pz[1] - z - pz[0]) * (pz[1] - z - 4 * f - pz[0]);
    CHECK_GT(c, 0);
    /* TODO
    << "Coefficient must be positive. It looks focusDistance: " << focusDistance << " is too short for a given lenses configuration";
    */
    Float delta = FLOAT_INV_2 * (pz[1] - z + pz[0] - std::sqrt(c));
    return elementInterfaces.back().thickness + delta;
}

Float RealisticCamera::FocusBinarySearch(Float focusDistance)
{
    Float filmDistanceLower, filmDistanceUpper;
    // Find _filmDistanceLower_, _filmDistanceUpper_ that bound focus distance
    filmDistanceLower = filmDistanceUpper = FocusThickLens(focusDistance);
    while (FocusDistance(filmDistanceLower) > focusDistance)
    {
        filmDistanceLower *= static_cast<Float>(1.005F);
    }
    while (FocusDistance(filmDistanceUpper) < focusDistance)
    {
        filmDistanceUpper /= static_cast<Float>(1.005F);
    }

    // Do binary search on film distances to focus
    for (int i = 0; i < 20; ++i)
    {
        Float fmid = FLOAT_INV_2 * (filmDistanceLower + filmDistanceUpper);
        Float midFocus = FocusDistance(fmid);
        if (midFocus < focusDistance)
        {
            filmDistanceLower = fmid;
        }
        else
        {
            filmDistanceUpper = fmid;
        }
    }

    return FLOAT_INV_2 * (filmDistanceLower + filmDistanceUpper);
}

Float RealisticCamera::FocusDistance(Float filmDistance)
{
    // Find offset ray from film center through lens
    common::math::Bounds2f bounds = BoundExitPupil(FLOAT_0, static_cast<Float>(0.001F) * film->diagonal);

    const std::array<Float, 3> scaleFactors
        = {static_cast<Float>(0.1F), static_cast<Float>(0.01F), static_cast<Float>(0.001F)};
    Float lu = FLOAT_0;

    common::math::Rayf ray;

    // Try some different and decreasing scaling factor to find focus ray
    // more quickly when `aperturediameter` is too small.
    // (e.g. 2 [mm] for `aperturediameter` with wide.22mm.dat),
    bool foundFocusRay = false;
    for (Float scale : scaleFactors)
    {
        lu = scale * bounds.point_max[0];
        if (TraceLensesFromFilm(common::math::Rayf(
            common::math::Vec3f(FLOAT_0, FLOAT_0, LensRearZ() - filmDistance),
            common::math::Vec3f(lu, FLOAT_0, filmDistance)), &ray))
        {
            foundFocusRay = true;
            break;
        }
    }

    if (!foundFocusRay)
    {
        /* TODO
        Error(
            "Focus ray at lens pos(%f,0) didn't make it through the lenses "
            "with film distance %f?!??\n",
            lu, filmDistance);
        */
        return (std::numeric_limits<Float>::max)();
    }

    // Compute distance _zFocus_ where ray intersects the principal axis
    Float tFocus = -ray.origin.x / ray.dir.x;
    Float zFocus = ray(tFocus).z;
    if (zFocus < FLOAT_0)
    {
        zFocus = (std::numeric_limits<Float>::max)();
    }
    return zFocus;
}


common::math::Bounds2f RealisticCamera::BoundExitPupil(Float pFilmX0, Float pFilmX1) const
{
    common::math::Bounds2f pupilBounds;
    // Sample a collection of points on the rear lens to find exit pupil
    const int nSamples = 1024 * 1024;
    int nExitingRays = 0;

    // Compute bounding box of projection of rear element on sampling plane
    Float rearRadius = RearElementRadius();
    common::math::Bounds2f projRearBounds(common::math::Vec2f(static_cast<Float>(-1.5F) * rearRadius
        , static_cast<Float>(-1.5F) * rearRadius),
        common::math::Vec2f(static_cast<Float>(1.5F) * rearRadius, static_cast<Float>(1.5F) * rearRadius));
    for (int i = 0; i < nSamples; ++i)
    {
        // Find location of sample points on $x$ segment and rear lens element
        common::math::Vec3f pFilm(common::math::Lerp((i + FLOAT_INV_2) / nSamples, pFilmX0, pFilmX1)
            , FLOAT_0, FLOAT_0);
        /* TODO
        Float u[2] = {core::sampler::RadicalInverse(0, i), core::sampler::RadicalInverse(1, i)};
        common::math::Vec3f pRear(common::math::Lerp(u[0], projRearBounds.point_min.x, projRearBounds.point_max.x),
            common::math::Lerp(u[1], projRearBounds.point_min.y, projRearBounds.point_max.y),
            LensRearZ());

        // Expand pupil bounds if ray makes it through the lens system
        if (Inside(common::math::Vec2f(pRear.x, pRear.y), pupilBounds) ||
            TraceLensesFromFilm(common::math::Rayf(pFilm, pRear - pFilm), nullptr))
        {
            pupilBounds = Union(pupilBounds, common::math::Vec2f(pRear.x, pRear.y));
            ++nExitingRays;
        }
        */
    }

    // Return entire element bounds if no rays made it through the lens system
    if (0 == nExitingRays)
    {
        /* TODO
        LOG(INFO) << StringPrintf("Unable to find exit pupil in x = [%f,%f] on film.",
            pFilmX0, pFilmX1);
        */
        return projRearBounds;
    }

    // Expand bounds to account for sample spacing
    pupilBounds = Expand(pupilBounds, FLOAT_2 * Length(projRearBounds.Diagonal()) /
        std::sqrt(nSamples));
    return pupilBounds;
}

void RealisticCamera::RenderExitPupil(Float sx, Float sy,
    const char *filename) const
{
    common::math::Vec3f pFilm(sx, sy, FLOAT_0);

    const int nSamples = 2048;
    Float *image = new Float[3 * nSamples * nSamples];
    Float *imagep = image;

    for (int y = 0; y < nSamples; ++y)
    {
        Float fy = static_cast<Float>(y) / static_cast<Float>(nSamples - 1);
        Float ly = common::math::Lerp(fy, -RearElementRadius(), RearElementRadius());
        for (int x = 0; x < nSamples; ++x)
        {
            Float fx = static_cast<Float>(x) / static_cast<Float>(nSamples - 1);
            Float lx = common::math::Lerp(fx, -RearElementRadius(), RearElementRadius());

            common::math::Vec3f pRear(lx, ly, LensRearZ());

            if (lx * lx + ly * ly > RearElementRadius() * RearElementRadius())
            {
                *imagep++ = FLOAT_1;
                *imagep++ = FLOAT_1;
                *imagep++ = FLOAT_1;
            }
            else if (TraceLensesFromFilm(common::math::Rayf(pFilm, pRear - pFilm), nullptr))
            {
                *imagep++ = FLOAT_INV_2;
                *imagep++ = FLOAT_INV_2;
                *imagep++ = FLOAT_INV_2;
            }
            else
            {
                *imagep++ = FLOAT_0;
                *imagep++ = FLOAT_0;
                *imagep++ = FLOAT_0;
            }
        }
    }

    /* TODO
    WriteImage(filename, image,
        common::math::Bounds2i(common::math::Vec2i(0, 0), common::math::Vec2i(nSamples, nSamples)),
        common::math::Vec2i(nSamples, nSamples));
        */
    delete[] image;
}

common::math::Vec3f RealisticCamera::SampleExitPupil(const common::math::Vec2f &pFilm,
    const common::math::Vec2f &lensSample,
    Float *sampleBoundsArea) const
{
    // Find exit pupil bound for sample distance from film center
    Float rFilm = std::sqrt(pFilm.x * pFilm.x + pFilm.y * pFilm.y);
    int rIndex = rFilm / (film->diagonal * FLOAT_INV_2) * exitPupilBounds.size();
    rIndex = (std::min)(static_cast<int>(exitPupilBounds.size()) - 1, rIndex);
    common::math::Bounds2f pupilBounds = exitPupilBounds[rIndex];
    if (sampleBoundsArea)
    {
        *sampleBoundsArea = pupilBounds.Area();
    }

    // Generate sample point inside exit pupil bound
    common::math::Vec2f pLens = pupilBounds.Lerp(lensSample);

    // Return sample point rotated by angle of _pFilm_ with $+x$ axis
    Float sinTheta = (rFilm != FLOAT_0) ? pFilm.y / rFilm : FLOAT_0;
    Float cosTheta = (rFilm != FLOAT_0) ? pFilm.x / rFilm : FLOAT_1;
    return common::math::Vec3f(cosTheta * pLens.x - sinTheta * pLens.y,
        sinTheta * pLens.x + cosTheta * pLens.y, LensRearZ());
}

/* TODO
void RealisticCamera::TestExitPupilBounds() const
{
    Float filmDiagonal = film->diagonal;

    static common::tool::RandomNumberGenerator rng;

    Float u = rng.UniformFloat();
    common::math::Vec3f pFilm(u * filmDiagonal * FLOAT_INV_2, FLOAT_0, FLOAT_0);

    Float r = pFilm.x / (filmDiagonal / 2);
    int pupilIndex =
        (std::min)((int)exitPupilBounds.size() - 1,
        (int)std::floor(r * (exitPupilBounds.size() - 1)));
    common::math::Bounds2f pupilBounds = exitPupilBounds[pupilIndex];
    if (pupilIndex + 1 < (int)exitPupilBounds.size())
    {
        pupilBounds = Union(pupilBounds, exitPupilBounds[pupilIndex + 1]);
    }

    // Now, randomly pick points on the aperture and see if any are outside
    // of pupil bounds...
    for (int i = 0; i < 1000; ++i)
    {
        common::math::Vec2f u2{rng.UniformFloat(), rng.UniformFloat()};
        common::math::Vec2f pd = sampler::ConcentricSampleDisk(u2);
        pd *= RearElementRadius();

        common::math::Rayf testRay(pFilm, common::math::Vec3f(pd.x, pd.y, FLOAT_0) - pFilm);
        common::math::Rayf testOut;
        if (!TraceLensesFromFilm(testRay, &testOut)) continue;

        if (!Inside(pd, pupilBounds))
        {
            fprintf(stderr,
                "Aha! (%f,%f) went through, but outside bounds (%f,%f) - "
                "(%f,%f)\n",
                pd.x, pd.y, pupilBounds.pMin[0], pupilBounds.pMin[1],
                pupilBounds.pMax[0], pupilBounds.pMax[1]);
            RenderExitPupil(
                (Float)pupilIndex / exitPupilBounds.size() * filmDiagonal / 2.f,
                0.f, "low.exr");
            RenderExitPupil((Float)(pupilIndex + 1) / exitPupilBounds.size() *
                filmDiagonal / 2.f,
                0.f, "high.exr");
            RenderExitPupil(pFilm.x, 0.f, "mid.exr");
            exit(0);
        }
    }
    fprintf(stderr, ".");
}
*/

Float RealisticCamera::GenerateRay(const CameraSample &sample, common::math::Rayf *ray) const
{
    //ProfilePhase prof(Prof::GenerateCameraRay);
    //++totalRays;
    // Find point on film, _pFilm_, corresponding to _sample.pFilm_
    common::math::Vec2f s(sample.pFilm.x / film->fullResolution.x,
        sample.pFilm.y / film->fullResolution.y);
    common::math::Vec2f pFilm2 = film->GetPhysicalExtent().Lerp(s);
    common::math::Vec3f pFilm(-pFilm2.x, pFilm2.y, FLOAT_0);

    // Trace ray from _pFilm_ through lens system
    Float exitPupilBoundsArea;
    common::math::Vec3f pRear = SampleExitPupil(common::math::Vec2f(pFilm.x, pFilm.y), sample.pLens,
        &exitPupilBoundsArea);
    common::math::Rayf rFilm(pFilm, pRear - pFilm, (std::numeric_limits<Float>::max)(), FLOAT_0,
        common::math::Lerp(sample.time, shutterOpen, shutterClose));
    if (!TraceLensesFromFilm(rFilm, ray))
    {
        //++vignettedRays;
        return FLOAT_0;
    }

    // Finish initialization of _RealisticCamera_ ray
    *ray = CameraToWorld(*ray);
    ray->dir = Normalize(ray->dir);
    ray->medium = medium;

    // Return weighting for _RealisticCamera_ ray
    Float cosTheta = Normalize(rFilm.dir).z;
    Float cos4Theta = (cosTheta * cosTheta) * (cosTheta * cosTheta);
    if (simpleWeighting)
    {
        return cos4Theta * exitPupilBoundsArea / exitPupilBounds[0].Area();
    }
    else
    {
        return (shutterClose - shutterOpen) *
            (cos4Theta * exitPupilBoundsArea) / (LensRearZ() * LensRearZ());
    }
}

/* TODO
RealisticCamera *CreateRealisticCamera(const ParamSet &params,
    const common::math::AnimatedTransformf &cam2world,
    core::film::Film *film, const core::interaction::Medium *medium)
{
    Float shutteropen = params.FindOneFloat("shutteropen", 0.f);
    Float shutterclose = params.FindOneFloat("shutterclose", 1.f);
    if (shutterclose < shutteropen)
    {
        Warning("Shutter close time [%f] < shutter open [%f].  Swapping them.",
            shutterclose, shutteropen);
        std::swap(shutterclose, shutteropen);
    }

    // Realistic camera-specific parameters
    std::string lensFile = params.FindOneFilename("lensfile", "");
    Float apertureDiameter = params.FindOneFloat("aperturediameter", 1.0);
    Float focusDistance = params.FindOneFloat("focusdistance", 10.0);
    bool simpleWeighting = params.FindOneBool("simpleweighting", true);
    if (lensFile == "")
    {
        Error("No lens description file supplied!");
        return nullptr;
    }
    // Load element data from lens description file
    std::vector<Float> lensData;
    if (!ReadFloatFile(lensFile.c_str(), &lensData))
    {
        Error("Error reading lens specification file \"%s\".",
            lensFile.c_str());
        return nullptr;
    }
    if (lensData.size() % 4 != 0)
    {
        Error(
            "Excess values in lens specification file \"%s\"; "
            "must be multiple-of-four values, read %d.",
            lensFile.c_str(), (int)lensData.size());
        return nullptr;
    }

    return new RealisticCamera(cam2world, shutteropen, shutterclose,
        apertureDiameter, focusDistance, simpleWeighting,
        lensData, film, medium);
}
*/


}
}