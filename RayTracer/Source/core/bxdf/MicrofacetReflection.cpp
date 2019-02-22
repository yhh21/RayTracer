#include "MicrofacetReflection.h"


namespace core
{
namespace bxdf
{


core::color::Spectrum MicrofacetReflection::f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
{
    Float cosThetaO = AbsCosTheta(wo), cosThetaI = AbsCosTheta(wi);
    common::math::Vec3f wh = wi + wo;
    // Handle degenerate cases for microfacet reflection
    if (FLOAT_0 == cosThetaI || FLOAT_0 == cosThetaO)
    {
        return core::color::Spectrum(FLOAT_0);
    }
    if (FLOAT_0 == wh.x && FLOAT_0 == wh.y && FLOAT_0 == wh.z)
    {
        return core::color::Spectrum(FLOAT_0);
    }
    wh = Normalize(wh);
    core::color::Spectrum F = fresnel->Evaluate(Dot(wi, wh));
    return R * distribution->D(wh) * distribution->G(wo, wi) * F /
        (FLOAT_2 * FLOAT_2 * cosThetaI * cosThetaO);
}

core::color::Spectrum MicrofacetReflection::Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi,
    const common::math::Vec2f &u, Float *pdf,
    BxDFType *sampledType) const
{
    // Sample microfacet orientation $\wh$ and reflected direction $\wi$
    if (FLOAT_0 == wo.z)
    {
        return FLOAT_0;
    }
    common::math::Vec3f wh = distribution->Sample_wh(wo, u);
    *wi = Reflect(wo, wh);
    if (!SameHemisphere(wo, *wi))
    {
        return core::color::Spectrum(FLOAT_0);
    }

    // Compute PDF of _wi_ for microfacet reflection
    *pdf = distribution->Pdf(wo, wh) / (FLOAT_2 * FLOAT_2 * Dot(wo, wh));
    return f(wo, *wi);
}

Float MicrofacetReflection::Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
{
    if (!SameHemisphere(wo, wi))
    {
        return FLOAT_0;
    }
    common::math::Vec3f wh = Normalize(wo + wi);
    return distribution->Pdf(wo, wh) / (FLOAT_2 * FLOAT_2 * Dot(wo, wh));
}


}
}