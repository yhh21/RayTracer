#include "DistantLight.h"
#include "VisibilityTester.h"
#include "../../common/math/Constants.h"
#include "../../common/math/Vec2.h"
#include "../bxdf/BxDF.h"
#include "../sampler/Sampling.h"

namespace core
{
namespace light
{


DistantLight::DistantLight(const common::math::Transformf &LightToWorld, const core::color::Spectrum &L,
    const common::math::Vec3f &wLight)
    : Light((int)LightFlags::DeltaDirection, LightToWorld, interaction::MediumInterface()),
    L(L),
    wLight(Normalize(LightToWorld(wLight)))
{}

core::color::Spectrum DistantLight::Sample_Li(const interaction::Interaction &ref, const common::math::Vec2f &u,
    common::math::Vec3f *wi, Float *pdf,
    VisibilityTester *vis) const
{
    //ProfilePhase _(Prof::LightSample);
    *wi = wLight;
    *pdf = FLOAT_1;
    common::math::Vec3f pOutside = ref.p + wLight * (FLOAT_2 * worldRadius);
    *vis = VisibilityTester(ref, interaction::Interaction(pOutside, ref.time, medium_interface));
    return L;
}

core::color::Spectrum DistantLight::Power() const
{
    return L * common::math::PI * worldRadius * worldRadius;
}

Float DistantLight::Pdf_Li(const interaction::Interaction &, const common::math::Vec3f &) const
{
    return FLOAT_0;
}

core::color::Spectrum DistantLight::Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2,
    Float time, common::math::Rayf *ray, common::math::Vec3f *nLight,
    Float *pdfPos, Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightSample);
    // Choose point on disk oriented toward infinite light direction
    common::math::Vec3f v1, v2;
    CoordinateSystem(wLight, &v1, &v2);
    common::math::Vec2f cd = sampler::ConcentricSampleDisk(u1);
    common::math::Vec3f pDisk = worldCenter + worldRadius * (cd.x * v1 + cd.y * v2);

    // Set ray origin and direction for infinite light ray
    *ray = common::math::Rayf(pDisk + worldRadius * wLight, -wLight, (std::numeric_limits<Float>::max)()
        , FLOAT_0, time);
    *nLight = (common::math::Vec3f)ray->dir;
    *pdfPos = FLOAT_1 / (common::math::PI * worldRadius * worldRadius);
    *pdfDir = FLOAT_1;
    return L;
}

void DistantLight::Pdf_Le(const common::math::Rayf &, const common::math::Vec3f &, Float *pdfPos,
    Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightPdf);
    *pdfPos = FLOAT_1 / (common::math::PI * worldRadius * worldRadius);
    *pdfDir = FLOAT_0;
}

/* TODO
std::shared_ptr<DistantLight> CreateDistantLight(const common::math::Transformf &light2world,
    const ParamSet &paramSet)
{
    core::color::Spectrum L = paramSet.FindOneSpectrum("L", core::color::Spectrum(1.0));
    core::color::Spectrum sc = paramSet.FindOneSpectrum("scale", core::color::Spectrum(1.0));
    common::math::Vec3f from = paramSet.FindOnePoint3f("from", common::math::Vec3f(0, 0, 0));
    common::math::Vec3f to = paramSet.FindOnePoint3f("to", common::math::Vec3f(0, 0, 1));
    common::math::Vec3f dir = from - to;
    return std::make_shared<DistantLight>(light2world, L * sc, dir);
}
*/


}
}