#include "FresnelBlend.h"
#include "./distribution/MicrofacetDistribution.h"
#include "../sampler/Sampling.h"

namespace core
{
namespace bxdf
{


FresnelBlend::FresnelBlend(const core::color::Spectrum &Rd, const core::color::Spectrum &Rs,
    bxdf::distribution::MicrofacetDistribution *distribution)
    : BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
    Rd(Rd),
    Rs(Rs),
    distribution(distribution)
{}

core::color::Spectrum FresnelBlend::f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
{
    auto pow5 = [](Float v)
    {
        return (v * v) * (v * v) * v;
    };
    core::color::Spectrum diffuse = (static_cast<Float>(28.0F) / (static_cast<Float>(23.0F) * common::math::PI))
        * Rd * (core::color::Spectrum(FLOAT_1) - Rs) *
        (FLOAT_1 - pow5(FLOAT_1 - FLOAT_INV_2 * AbsCosTheta(wi))) *
        (FLOAT_1 - pow5(FLOAT_1 - FLOAT_INV_2 * AbsCosTheta(wo)));
    common::math::Vec3f wh = wi + wo;
    if (FLOAT_0 == wh.x && FLOAT_0 == wh.y && FLOAT_0 == wh.z)
    {
        return core::color::Spectrum(FLOAT_0);
    }
    wh = Normalize(wh);
    core::color::Spectrum specular = distribution->D(wh) /
        (static_cast<Float>(4.0F) * AbsDot(wi, wh) * (std::max)(AbsCosTheta(wi), AbsCosTheta(wo))) *
        SchlickFresnel(Dot(wi, wh));
    return diffuse + specular;
}

core::color::Spectrum FresnelBlend::Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi,
    const common::math::Vec2f &uOrig, Float *pdf,
    BxDFType *sampledType) const
{
    common::math::Vec2f u = uOrig;
    if (u[0] < FLOAT_INV_2)
    {
        u[0] = (std::min)(FLOAT_2 * u[0], common::math::ONE_MINUS_MACHINE_EPSILON);
        // Cosine-sample the hemisphere, flipping the direction if necessary
        *wi = core::sampler::CosineSampleHemisphere(u);
        if (wo.z < FLOAT_0)
        {
            wi->z *= -FLOAT_1;
        }
    }
    else
    {
        u[0] = (std::min)(FLOAT_2 * (u[0] - FLOAT_INV_2), common::math::ONE_MINUS_MACHINE_EPSILON);
        // Sample microfacet orientation $\wh$ and reflected direction $\wi$
        common::math::Vec3f wh = distribution->Sample_wh(wo, u);
        *wi = Reflect(wo, wh);
        if (!SameHemisphere(wo, *wi))
        {
            return core::color::Spectrum(FLOAT_0);
        }
    }
    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}

Float FresnelBlend::Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
{
    if (!SameHemisphere(wo, wi))
    {
        return FLOAT_0;
    }
    common::math::Vec3f wh = Normalize(wo + wi);
    Float pdf_wh = distribution->Pdf(wo, wh);
    return FLOAT_INV_2 * (AbsCosTheta(wi) * common::math::INV_PI
        + pdf_wh / (static_cast<Float>(4.0F) * Dot(wo, wh)));
}


}
}