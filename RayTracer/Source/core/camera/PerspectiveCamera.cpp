#include "PerspectiveCamera.h"

#include "../film/Film.h"
#include "../interaction/Interaction.h"
#include "../light/VisibilityTester.h"
#include "../sampler/Sampling.h"
#include "../../common/math/Constants.h"
#include "../../common/math/Ray.h"
#include "../../common/math/RayDifferential.h"
#include "../../common/math/Vec3.h"

namespace core
{
namespace camera
{


PerspectiveCamera::PerspectiveCamera(const common::math::AnimatedTransformf &CameraToWorld,
    const common::math::Bounds2f &screenWindow,
    Float shutterOpen, Float shutterClose,
    Float lensRadius, Float focalDistance,
    Float fov, core::film::Film *film,
    const core::interaction::Medium *medium)
    : ProjectiveCamera(CameraToWorld, common::math::Perspective(fov, static_cast<Float>(1e-2f)
        , static_cast<Float>(1000.0F)),
        screenWindow, shutterOpen, shutterClose, lensRadius,
        focalDistance, film, medium)
{
    // Compute differential changes in origin for perspective camera rays
    dxCamera = (RasterToCamera(common::math::Vec3f(FLOAT_1, FLOAT_0, FLOAT_0))
            - RasterToCamera(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0)));
    dyCamera = (RasterToCamera(common::math::Vec3f(FLOAT_0, FLOAT_1, FLOAT_0))
        - RasterToCamera(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0)));

    // Compute image plane bounds at $z=1$ for _PerspectiveCamera_
    common::math::Vec2i res = film->fullResolution;
    common::math::Vec3f pMin = RasterToCamera(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0));
    common::math::Vec3f pMax = RasterToCamera(common::math::Vec3f(res.x, res.y, FLOAT_0));
    pMin /= pMin.z;
    pMax /= pMax.z;
    A = std::abs((pMax.x - pMin.x) * (pMax.y - pMin.y));
}

Float PerspectiveCamera::GenerateRay(const CameraSample &sample,
    common::math::Rayf *ray) const
{
    //ProfilePhase prof(Prof::GenerateCameraRay);
    // Compute raster and camera sample positions
    common::math::Vec3f pFilm = common::math::Vec3f(sample.pFilm.x, sample.pFilm.y, FLOAT_0);
    common::math::Vec3f pCamera = RasterToCamera(pFilm);
    *ray = common::math::Rayf(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0)
        , Normalize(common::math::Vec3f(pCamera)));
    // Modify ray for depth of field
    if (lensRadius > FLOAT_0)
    {
        // Sample point on lens
        common::math::Vec2f pLens = lensRadius * sampler::ConcentricSampleDisk(sample.pLens);

        // Compute point on plane of focus
        Float ft = focalDistance / ray->dir.z;
        common::math::Vec3f pFocus = (*ray)(ft);

        // Update ray for effect of lens
        ray->origin = common::math::Vec3f(pLens.x, pLens.y, FLOAT_0);
        ray->dir = Normalize(pFocus - ray->origin);
    }
    ray->time = common::math::Lerp(sample.time, shutterOpen, shutterClose);
    ray->medium = medium;
    *ray = CameraToWorld(*ray);
    return FLOAT_1;
}

Float PerspectiveCamera::GenerateRayDifferential(const CameraSample &sample,
    common::math::RayDifferentialf *ray) const
{
    //ProfilePhase prof(Prof::GenerateCameraRay);
    // Compute raster and camera sample positions
    common::math::Vec3f pFilm = common::math::Vec3f(sample.pFilm.x, sample.pFilm.y, FLOAT_0);
    common::math::Vec3f pCamera = RasterToCamera(pFilm);
    common::math::Vec3f dir = Normalize(common::math::Vec3f(pCamera.x, pCamera.y, pCamera.z));
    *ray = common::math::RayDifferentialf(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0), dir);
    // Modify ray for depth of field
    if (lensRadius > FLOAT_0)
    {
        // Sample point on lens
        common::math::Vec2f pLens = lensRadius * sampler::ConcentricSampleDisk(sample.pLens);

        // Compute point on plane of focus
        Float ft = focalDistance / ray->dir.z;
        common::math::Vec3f pFocus = (*ray)(ft);

        // Update ray for effect of lens
        ray->origin = common::math::Vec3f(pLens.x, pLens.y, FLOAT_0);
        ray->dir = Normalize(pFocus - ray->origin);
    }

    // Compute offset rays for _PerspectiveCamera_ ray differentials
    if (lensRadius > FLOAT_0)
    {
        // Compute _PerspectiveCamera_ ray differentials accounting for lens

        // Sample point on lens
        common::math::Vec2f pLens = lensRadius * sampler::ConcentricSampleDisk(sample.pLens);
        common::math::Vec3f dx = Normalize(common::math::Vec3f(pCamera + dxCamera));
        Float ft = focalDistance / dx.z;
        common::math::Vec3f pFocus = common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0) + (ft * dx);
        ray->rx_origin = common::math::Vec3f(pLens.x, pLens.y, FLOAT_0);
        ray->rx_direction = Normalize(pFocus - ray->rx_origin);

        common::math::Vec3f dy = Normalize(common::math::Vec3f(pCamera + dyCamera));
        ft = focalDistance / dy.z;
        pFocus = common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0) + (ft * dy);
        ray->ry_origin = common::math::Vec3f(pLens.x, pLens.y, FLOAT_0);
        ray->ry_direction = Normalize(pFocus - ray->ry_origin);
    }
    else
    {
        ray->rx_origin = ray->ry_origin = ray->origin;
        ray->rx_direction = Normalize(common::math::Vec3f(pCamera) + dxCamera);
        ray->ry_direction = Normalize(common::math::Vec3f(pCamera) + dyCamera);
    }
    ray->time = common::math::Lerp(sample.time, shutterOpen, shutterClose);
    ray->medium = medium;
    *ray = CameraToWorld(*ray);
    ray->has_differentials = true;
    return FLOAT_1;
}

core::color::Spectrum PerspectiveCamera::We(const common::math::Rayf &ray, common::math::Vec2f *pRaster2) const
{
    // Interpolate camera matrix and check if $\w{}$ is forward-facing
    common::math::Transformf c2w;
    CameraToWorld.Interpolate(ray.time, &c2w);
    Float cosTheta = Dot(ray.dir, c2w(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_1)));
    if (cosTheta <= FLOAT_0)
    {
        return core::color::Spectrum(FLOAT_0);
    }

    // Map ray $(\p{}, \w{})$ onto the raster grid
    common::math::Vec3f pFocus = ray((lensRadius > FLOAT_0 ? focalDistance : FLOAT_1) / cosTheta);
    common::math::Vec3f pRaster = Inverse(RasterToCamera)(Inverse(c2w)(pFocus));

    // Return raster position if requested
    if (pRaster2)
    {
        *pRaster2 = common::math::Vec2f(pRaster.x, pRaster.y);
    }

    // Return zero importance for out of bounds points
    common::math::Bounds2i sampleBounds = film->GetSampleBounds();
    if (pRaster.x < sampleBounds.point_min.x || pRaster.x >= sampleBounds.point_max.x ||
        pRaster.y < sampleBounds.point_min.y || pRaster.y >= sampleBounds.point_max.y)
    {
        return core::color::Spectrum(FLOAT_0);
    }

    // Compute lens area of perspective camera
    Float lensArea = lensRadius != FLOAT_0 ? (common::math::PI * lensRadius * lensRadius) : FLOAT_1;

    // Return importance for point on image plane
    Float cos2Theta = cosTheta * cosTheta;
    return core::color::Spectrum(FLOAT_1 / (A * lensArea * cos2Theta * cos2Theta));
}

void PerspectiveCamera::Pdf_We(const common::math::Rayf &ray, Float *pdfPos,
    Float *pdfDir) const
{
    // Interpolate camera matrix and fail if $\w{}$ is not forward-facing
    common::math::Transformf c2w;
    CameraToWorld.Interpolate(ray.time, &c2w);
    Float cosTheta = Dot(ray.dir, c2w(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_1)));
    if (cosTheta <= FLOAT_0)
    {
        *pdfPos = *pdfDir = FLOAT_0;
        return;
    }

    // Map ray $(\p{}, \w{})$ onto the raster grid
    common::math::Vec3f pFocus = ray((lensRadius > FLOAT_0 ? focalDistance : FLOAT_1) / cosTheta);
    common::math::Vec3f pRaster = Inverse(RasterToCamera)(Inverse(c2w)(pFocus));

    // Return zero probability for out of bounds points
    common::math::Bounds2i sampleBounds = film->GetSampleBounds();
    if (pRaster.x < sampleBounds.point_min.x || pRaster.x >= sampleBounds.point_max.x ||
        pRaster.y < sampleBounds.point_min.y || pRaster.y >= sampleBounds.point_max.y)
    {
        *pdfPos = *pdfDir = FLOAT_0;
        return;
    }

    // Compute lens area of perspective camera
    Float lensArea = lensRadius != FLOAT_0 ? (common::math::PI * lensRadius * lensRadius) : FLOAT_1;
    *pdfPos = FLOAT_1 / lensArea;
    *pdfDir = FLOAT_1 / (A * cosTheta * cosTheta * cosTheta);
}

core::color::Spectrum PerspectiveCamera::Sample_Wi(const core::interaction::Interaction &ref, const common::math::Vec2f &u,
    common::math::Vec3f *wi, Float *pdf,
    common::math::Vec2f *pRaster,
    core::light::VisibilityTester *vis) const
{
    // Uniformly sample a lens interaction _lensIntr_
    common::math::Vec2f pLens = lensRadius * sampler::ConcentricSampleDisk(u);
    common::math::Vec3f pLensWorld = CameraToWorld(ref.time, common::math::Vec3f(pLens.x, pLens.y, FLOAT_0));
    core::interaction::Interaction lensIntr(pLensWorld, ref.time, medium);
    lensIntr.n = common::math::Vec3f(CameraToWorld(ref.time, common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_1)));

    // Populate arguments and compute the importance value
    *vis = core::light::VisibilityTester(ref, lensIntr);
    *wi = lensIntr.p - ref.p;
    Float dist = Length(*wi);
    *wi /= dist;

    // Compute PDF for importance arriving at _ref_

    // Compute lens area of perspective camera
    Float lensArea = lensRadius != FLOAT_0 ? (common::math::PI * lensRadius * lensRadius) : FLOAT_1;
    *pdf = (dist * dist) / (AbsDot(lensIntr.n, *wi) * lensArea);
    return We(lensIntr.SpawnRay(-*wi), pRaster);
}

/* TODO
PerspectiveCamera *CreatePerspectiveCamera(const ParamSet &params,
    const common::math::AnimatedTransformf &cam2world,
    core::film::Film *film, const core::interaction::Medium *medium)
{
    // Extract common camera parameters from _ParamSet_
    Float shutteropen = params.FindOneFloat("shutteropen", 0.f);
    Float shutterclose = params.FindOneFloat("shutterclose", 1.f);
    if (shutterclose < shutteropen)
    {
        Warning("Shutter close time [%f] < shutter open [%f].  Swapping them.",
            shutterclose, shutteropen);
        std::swap(shutterclose, shutteropen);
    }
    Float lensradius = params.FindOneFloat("lensradius", 0.f);
    Float focaldistance = params.FindOneFloat("focaldistance", 1e6);
    Float frame = params.FindOneFloat(
        "frameaspectratio",
        Float(film->fullResolution.x) / Float(film->fullResolution.y));
    common::math::Bounds2f screen;
    if (frame > 1.f)
    {
        screen.pMin.x = -frame;
        screen.pMax.x = frame;
        screen.pMin.y = -1.f;
        screen.pMax.y = 1.f;
    }
    else
    {
        screen.pMin.x = -1.f;
        screen.pMax.x = 1.f;
        screen.pMin.y = -1.f / frame;
        screen.pMax.y = 1.f / frame;
    }
    int swi;
    const Float *sw = params.FindFloat("screenwindow", &swi);
    if (sw)
    {
        if (4 == swi)
        {
            screen.pMin.x = sw[0];
            screen.pMax.x = sw[1];
            screen.pMin.y = sw[2];
            screen.pMax.y = sw[3];
        }
        else
            Error("\"screenwindow\" should have four values");
    }
    Float fov = params.FindOneFloat("fov", 90.);
    Float halffov = params.FindOneFloat("halffov", -1.f);
    if (halffov > 0.f)
        // hack for structure synth, which exports half of the full fov
        fov = 2.f * halffov;
    return new PerspectiveCamera(cam2world, screen, shutteropen, shutterclose,
        lensradius, focaldistance, fov, film, medium);
}
*/


}
}