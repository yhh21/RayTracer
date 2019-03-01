#include "MicrofacetTransmission.h"

namespace core
{
namespace bxdf
{


core::color::Spectrum MicrofacetTransmission::f(const common::math::Vec3f &wo,
    const common::math::Vec3f &wi) const
{
    if (SameHemisphere(wo, wi))
    {
        return FLOAT_0;  // transmission only
    }

    Float cosThetaO = CosTheta(wo);
    Float cosThetaI = CosTheta(wi);
    if (FLOAT_0 == cosThetaI || FLOAT_0 == cosThetaO)
    {
        return core::color::Spectrum(FLOAT_0);
    }

    // Compute $\wh$ from $\wo$ and $\wi$ for microfacet transmission
    Float eta = CosTheta(wo) > FLOAT_0 ? (etaB / etaA) : (etaA / etaB);
    common::math::Vec3f wh = Normalize(wo + wi * eta);
    if (wh.z < FLOAT_0)
    {
        wh = -wh;
    }

    core::color::Spectrum F = fresnel.Evaluate(Dot(wo, wh));

    Float sqrtDenom = Dot(wo, wh) + eta * Dot(wi, wh);
    Float factor = (core::material::TransportMode::Radiance == mode) ? (FLOAT_1 / eta) : FLOAT_1;

    return (core::color::Spectrum(FLOAT_1) - F) * T *
        std::abs(distribution->D(wh) * distribution->G(wo, wi) * eta * eta *
            AbsDot(wi, wh) * AbsDot(wo, wh) * factor * factor /
            (cosThetaI * cosThetaO * sqrtDenom * sqrtDenom));
}

core::color::Spectrum MicrofacetTransmission::Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi,
    const common::math::Vec2f &u, Float *pdf,
    BxDFType *sampledType) const
{
    if (FLOAT_0 == wo.z)
    {
        return FLOAT_0;
    }
    common::math::Vec3f wh = distribution->Sample_wh(wo, u);
    Float eta = CosTheta(wo) > FLOAT_0 ? (etaA / etaB) : (etaB / etaA);
    if (!Refract(wo, (common::math::Vec3f)wh, eta, wi))
    {
        return FLOAT_0;
    }
    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}

Float MicrofacetTransmission::Pdf(const common::math::Vec3f &wo,
    const common::math::Vec3f &wi) const
{
    if (SameHemisphere(wo, wi))
    {
        return FLOAT_0;
    }
    // Compute $\wh$ from $\wo$ and $\wi$ for microfacet transmission
    Float eta = CosTheta(wo) > FLOAT_0 ? (etaB / etaA) : (etaA / etaB);
    common::math::Vec3f wh = Normalize(wo + wi * eta);

    // Compute change of variables _dwh\_dwi_ for microfacet transmission
    Float sqrtDenom = Dot(wo, wh) + eta * Dot(wi, wh);
    Float dwh_dwi = std::abs((eta * eta * Dot(wi, wh)) / (sqrtDenom * sqrtDenom));
    return distribution->Pdf(wo, wh) * dwh_dwi;
}


}
}