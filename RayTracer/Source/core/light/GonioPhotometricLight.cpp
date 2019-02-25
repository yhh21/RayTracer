#include "GonioPhotometricLight.h"
#include "VisibilityTester.h"
#include "../../common/math/Constants.h"
#include "../../common/math/Vec2.h"
#include "../bxdf/BxDF.h"
#include "../sampler/Sampling.h"

namespace core
{
namespace light
{

core::color::Spectrum GonioPhotometricLight::Sample_Li(const interaction::Interaction &ref,
    const common::math::Vec2f &u, common::math::Vec3f *wi,
    Float *pdf,
    VisibilityTester *vis) const
{
    //ProfilePhase _(Prof::LightSample);
    *wi = Normalize(pLight - ref.p);
    *pdf = FLOAT_1;
    *vis = VisibilityTester(ref, interaction::Interaction(pLight, ref.time, medium_interface));
    return I * Scale(-*wi) / DistanceSquared(pLight, ref.p);
}

core::color::Spectrum GonioPhotometricLight::Power() const
{
    return static_cast<Float>(4.0F) * common::math::PI * I * core::color::Spectrum(mipmap
        ? mipmap->Lookup(common::math::Vec2f(FLOAT_INV_2, FLOAT_INV_2), FLOAT_INV_2)
        : core::color::RGBSpectrum(FLOAT_1),
        core::color::SpectrumType::Illuminant);
}

Float GonioPhotometricLight::Pdf_Li(const interaction::Interaction &,
    const common::math::Vec3f &) const
{
    return FLOAT_0;
}

core::color::Spectrum GonioPhotometricLight::Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2,
    Float time, common::math::Rayf *ray,
    common::math::Vec3f *nLight, Float *pdfPos,
    Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightSample);
    *ray = common::math::Rayf(pLight, sampler::UniformSampleSphere(u1)
        , (std::numeric_limits<Float>::max)(), FLOAT_0, time,
        medium_interface.inside);
    *nLight = (common::math::Vec3f)ray->dir;
    *pdfPos = FLOAT_1;
    *pdfDir = sampler::UniformSpherePdf();
    return I * Scale(ray->dir);
}

void GonioPhotometricLight::Pdf_Le(const common::math::Rayf &, const common::math::Vec3f &, Float *pdfPos,
    Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightPdf);
    *pdfPos = FLOAT_0;
    *pdfDir = sampler::UniformSpherePdf();
}

/* TODO
std::shared_ptr<GonioPhotometricLight> CreateGoniometricLight(
    const common::math::Transformf &light2world, const Medium *medium,
    const ParamSet &paramSet)
{
    core::color::Spectrum I = paramSet.FindOneSpectrum("I", core::color::Spectrum(1.0));
    core::color::Spectrum sc = paramSet.FindOneSpectrum("scale", core::color::Spectrum(1.0));
    std::string texname = paramSet.FindOneFilename("mapname", "");
    return std::make_shared<GonioPhotometricLight>(light2world, medium, I * sc,
        texname);
}
*/


}
}