#include "DiffuseAreaLight.h"
#include "VisibilityTester.h"
#include "../../common/math/Constants.h"
#include "../../common/math/Vec2.h"
#include "../bxdf/BxDF.h"
#include "../sampler/Sampling.h"
#include "../shape/Triangle.h"

namespace core
{
namespace light
{


DiffuseAreaLight::DiffuseAreaLight(const common::math::Transformf &LightToWorld,
    const interaction::MediumInterface &medium_interface,
    const core::color::Spectrum &Lemit, int nSamples,
    const std::shared_ptr<core::shape::Shape> &shape,
    bool twoSided)
    : AreaLight(LightToWorld, medium_interface, nSamples),
    Lemit(Lemit),
    shape(shape),
    twoSided(twoSided),
    area(shape->Area())
{
    // Warn if light has transformation with non-uniform scale, though not
    // for Triangles, since this doesn't matter for them.
    if (WorldToLight.HasScale() &&
        dynamic_cast<const core::shape::Triangle *>(shape.get()) == nullptr)
    {
        /* TODO
        Warning(
            "Scaling detected in world to light transformation! "
            "The system has numerous assumptions, implicit and explicit, "
            "that this transform will have no scale factors in it. "
            "Proceed at your own risk; your image may have errors.");
            */
    }
}

core::color::Spectrum DiffuseAreaLight::Power() const
{
    return (twoSided ? FLOAT_2 : FLOAT_1) * Lemit * area * common::math::PI;
}

core::color::Spectrum DiffuseAreaLight::Sample_Li(const interaction::Interaction &ref, const common::math::Vec2f &u,
    common::math::Vec3f *wi, Float *pdf,
    VisibilityTester *vis) const
{
    //ProfilePhase _(Prof::LightSample);
    interaction::Interaction pShape = shape->Sample(ref, u, pdf);
    pShape.medium_interface = medium_interface;
    if (FLOAT_0 == *pdf || FLOAT_0 == LengthSquared(pShape.p - ref.p))
    {
        *pdf = FLOAT_0;
        return core::color::Spectrum(FLOAT_0);
    }
    *wi = Normalize(pShape.p - ref.p);
    *vis = VisibilityTester(ref, pShape);
    return L(pShape, -*wi);
}

Float DiffuseAreaLight::Pdf_Li(const interaction::Interaction &ref,
    const common::math::Vec3f &wi) const
{
    //ProfilePhase _(Prof::LightPdf);
    return shape->Pdf(ref, wi);
}

core::color::Spectrum DiffuseAreaLight::Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2,
    Float time, common::math::Rayf *ray, common::math::Vec3f *nLight,
    Float *pdfPos, Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightSample);
    // Sample a point on the area light's _Shape_, _pShape_
    interaction::Interaction pShape = shape->Sample(u1, pdfPos);
    pShape.medium_interface = medium_interface;
    *nLight = pShape.n;

    // Sample a cosine-weighted outgoing direction _w_ for area light
    common::math::Vec3f w;
    if (twoSided)
    {
        common::math::Vec2f u = u2;
        // Choose a side to sample and then remap u[0] to [0,1] before
        // applying cosine-weighted hemisphere sampling for the chosen side.
        if (u[0] < FLOAT_INV_2)
        {
            u[0] = (std::min)(u[0] * FLOAT_2, common::math::ONE_MINUS_MACHINE_EPSILON);
            w = sampler::CosineSampleHemisphere(u);
        }
        else
        {
            u[0] = (std::min)((u[0] - FLOAT_INV_2) * FLOAT_2, common::math::ONE_MINUS_MACHINE_EPSILON);
            w = sampler::CosineSampleHemisphere(u);
            w.z *= -FLOAT_1;
        }
        *pdfDir = FLOAT_INV_2 * sampler::CosineHemispherePdf(std::abs(w.z));
    }
    else
    {
        w = sampler::CosineSampleHemisphere(u2);
        *pdfDir = sampler::CosineHemispherePdf(w.z);
    }

    common::math::Vec3f v1, v2, n(pShape.n);
    CoordinateSystem(n, &v1, &v2);
    w = w.x * v1 + w.y * v2 + w.z * n;
    *ray = pShape.SpawnRay(w);
    return L(pShape, w);
}

void DiffuseAreaLight::Pdf_Le(const common::math::Rayf &ray, const common::math::Vec3f &n, Float *pdfPos,
    Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightPdf);
    interaction::Interaction it(ray.origin, n, common::math::Vec3f(), common::math::Vec3f(n), ray.time,
        medium_interface);
    *pdfPos = shape->Pdf(it);
    *pdfDir = twoSided ? (FLOAT_INV_2 * sampler::CosineHemispherePdf(AbsDot(n, ray.dir)))
        : sampler::CosineHemispherePdf(Dot(n, ray.dir));
}

/* TODO
std::shared_ptr<AreaLight> CreateDiffuseAreaLight(
    const common::math::Transformf &light2world, const Medium *medium,
    const ParamSet &paramSet, const std::shared_ptr<core::shape::Shape> &shape)
{
    core::color::Spectrum L = paramSet.FindOneSpectrum("L", core::color::Spectrum(1.0));
    core::color::Spectrum sc = paramSet.FindOneSpectrum("scale", core::color::Spectrum(1.0));
    int nSamples = paramSet.FindOneInt("samples",
        paramSet.FindOneInt("nsamples", 1));
    bool twoSided = paramSet.FindOneBool("twosided", false);
    if (PbrtOptions.quickRender) nSamples = (std::min)(1, nSamples / 4);
    return std::make_shared<DiffuseAreaLight>(light2world, medium, L * sc,
        nSamples, shape, twoSided);
}
*/


}
}