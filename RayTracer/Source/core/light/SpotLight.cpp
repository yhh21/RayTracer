#include "SpotLight.h"
#include "VisibilityTester.h"
#include "../../common/math/Constants.h"
#include "../../common/math/Vec2.h"
#include "../bxdf/BxDF.h"
#include "../interaction/MediumInteraction.h"
#include "../sampler/Sampling.h"

namespace core
{
namespace light
{


SpotLight::SpotLight(const common::math::Transformf &LightToWorld,
    const core::interaction::MediumInterface &medium_interface, const core::color::Spectrum &I,
    Float totalWidth, Float falloffStart)
    : Light(static_cast<int>(LightFlags::DeltaPosition), LightToWorld, medium_interface),
    pLight(LightToWorld(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0))),
    I(I),
    cosTotalWidth(std::cos(common::math::ConvertToRadians(totalWidth))),
    cosFalloffStart(std::cos(common::math::ConvertToRadians(falloffStart)))
{}

core::color::Spectrum SpotLight::Sample_Li(const interaction::Interaction &ref, const common::math::Vec2f &u,
    common::math::Vec3f *wi, Float *pdf,
    VisibilityTester *vis) const
{
    //ProfilePhase _(Prof::LightSample);
    *wi = Normalize(pLight - ref.p);
    *pdf = FLOAT_1;
    *vis = VisibilityTester(ref, interaction::Interaction(pLight, ref.time, medium_interface));
    return I * Falloff(-*wi) / DistanceSquared(pLight, ref.p);
}

Float SpotLight::Falloff(const common::math::Vec3f &w) const
{
    common::math::Vec3f wl = Normalize(WorldToLight(w));
    Float cosTheta = wl.z;
    if (cosTheta < cosTotalWidth)
    {
        return FLOAT_0;
    }
    if (cosTheta >= cosFalloffStart)
    {
        return FLOAT_1;
    }
    // Compute falloff inside spotlight cone
    Float delta = (cosTheta - cosTotalWidth) / (cosFalloffStart - cosTotalWidth);
    return (delta * delta) * (delta * delta);
}

core::color::Spectrum SpotLight::Power() const
{
    return I * FLOAT_2 * common::math::PI * (FLOAT_1 - FLOAT_INV_2 * (cosFalloffStart + cosTotalWidth));
}

Float SpotLight::Pdf_Li(const interaction::Interaction &, const common::math::Vec3f &) const
{
    return FLOAT_0;
}

core::color::Spectrum SpotLight::Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2, Float time,
    common::math::Rayf *ray, common::math::Vec3f *nLight, Float *pdfPos,
    Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightSample);
    common::math::Vec3f w = sampler::UniformSampleCone(u1, cosTotalWidth);
    *ray = common::math::Rayf(pLight, LightToWorld(w), (std::numeric_limits<Float>::max)(), FLOAT_0
        , time, medium_interface.inside);
    *nLight = static_cast<common::math::Vec3f>(ray->dir);
    *pdfPos = FLOAT_1;
    *pdfDir = sampler::UniformConePdf(cosTotalWidth);
    return I * Falloff(ray->dir);
}

void SpotLight::Pdf_Le(const common::math::Rayf &ray, const common::math::Vec3f &, Float *pdfPos,
    Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightPdf);
    *pdfPos = FLOAT_0;
    *pdfDir = (bxdf::CosTheta(WorldToLight(ray.dir)) >= cosTotalWidth)
        ? sampler::UniformConePdf(cosTotalWidth)
        : FLOAT_0;
}

/* TODO
std::shared_ptr<SpotLight> CreateSpotLight(const common::math::Transformf &l2w,
    const Medium *medium,
    const ParamSet &paramSet)
{
    core::color::Spectrum I = paramSet.FindOneSpectrum("I", core::color::Spectrum(1.0));
    core::color::Spectrum sc = paramSet.FindOneSpectrum("scale", core::color::Spectrum(1.0));
    Float coneangle = paramSet.FindOneFloat("coneangle", 30.);
    Float conedelta = paramSet.FindOneFloat("conedeltaangle", 5.);
    // Compute spotlight world to light transformation
    common::math::Vec3f from = paramSet.FindOnePoint3f("from", common::math::Vec3f(0, 0, 0));
    common::math::Vec3f to = paramSet.FindOnePoint3f("to", common::math::Vec3f(0, 0, 1));
    common::math::Vec3f dir = Normalize(to - from);
    common::math::Vec3f du, dv;
    CoordinateSystem(dir, &du, &dv);
    common::math::Transformf dirToZ =
        common::math::Transformf(Matrix4x4(du.x, du.y, du.z, 0., dv.x, dv.y, dv.z, 0., dir.x,
            dir.y, dir.z, 0., 0, 0, 0, 1.));
    common::math::Transformf light2world =
        l2w * Translate(common::math::Vec3f(from.x, from.y, from.z)) * Inverse(dirToZ);
    return std::make_shared<SpotLight>(light2world, medium, I * sc, coneangle,
        coneangle - conedelta);
}
*/


}
}