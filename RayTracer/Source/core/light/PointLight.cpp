#include "PointLight.h"
#include "VisibilityTester.h"
#include "../../common/math/Constants.h"
#include "../sampler/Sampling.h"
#include "../interaction/MediumInteraction.h"

namespace core
{
namespace light
{


PointLight::PointLight(const common::math::Transformf &LightToWorld,
    const core::interaction::MediumInterface &medium_interface, const color::Spectrum &I)
    : Light(static_cast<int>(LightFlags::DeltaPosition), LightToWorld, medium_interface),
    pLight(LightToWorld(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0))),
    I(I)
{}

color::Spectrum PointLight::Sample_Li(const core::interaction::Interaction &ref, const common::math::Vec2f &u,
    common::math::Vec3f *wi, Float *pdf,
    VisibilityTester *vis) const
{
    //ProfilePhase _(Prof::LightSample);
    *wi = Normalize(pLight - ref.p);
    *pdf = FLOAT_1;
    *vis = VisibilityTester(ref, core::interaction::Interaction(pLight, ref.time, medium_interface));
    return I / DistanceSquared(pLight, ref.p);
}

color::Spectrum PointLight::Power() const
{
    return FLOAT_2 * common::math::TWO_PI * I;
}

Float PointLight::Pdf_Li(const core::interaction::Interaction &, const common::math::Vec3f &) const
{
    return FLOAT_0;
}

color::Spectrum PointLight::Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2, Float time,
    common::math::Rayf *ray, common::math::Vec3f *nLight, Float *pdfPos,
    Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightSample);
    *ray = common::math::Rayf(pLight, core::sampler::UniformSampleSphere(u1), (std::numeric_limits<Float>::max)(), FLOAT_0, time,
        medium_interface.inside);
    *nLight = (common::math::Vec3f)ray->dir;
    *pdfPos = FLOAT_1;
    *pdfDir = core::sampler::UniformSpherePdf();
    return I;
}

void PointLight::Pdf_Le(const common::math::Rayf &, const common::math::Vec3f &, Float *pdfPos,
    Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightPdf);
    *pdfPos = FLOAT_0;
    *pdfDir = core::sampler::UniformSpherePdf();
}


/* TODO
std::shared_ptr<PointLight> CreatePointLight(const common::math::Transformf &light2world,
    const Medium *medium,
    const ParamSet &paramSet)
{
    color::Spectrum I = paramSet.FindOneSpectrum("I", color::Spectrum(1.0));
    color::Spectrum sc = paramSet.FindOneSpectrum("scale", color::Spectrum(1.0));
    common::math::Vec3f P = paramSet.FindOnePoint3f("from", common::math::Vec3f(0, 0, 0));
    common::math::Transformf l2w = Translate(common::math::Vec3f(P.x, P.y, P.z)) * light2world;
    return std::make_shared<PointLight>(l2w, medium, I * sc);
}
*/


}
}