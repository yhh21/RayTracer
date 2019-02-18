#include "Fresnel.h"
#include "../../common/math/Constants.h"

namespace core
{
namespace bxdf
{


color::Spectrum FresnelConductor::Evaluate(Float cosThetaI) const
{
    return FrConductor(std::abs(cosThetaI), etaI, etaT, k);
}

color::Spectrum FresnelDielectric::Evaluate(Float cosThetaI) const
{
    return FrDielectric(cosThetaI, etaI, etaT);
}

color::Spectrum FresnelNoOp::Evaluate(Float cosThetaI) const
{
    return color::Spectrum(FLOAT_1);
}


Float FrDielectric(Float cosThetaI, Float etaI, Float etaT)
{
    cosThetaI = common::math::Clamp(cosThetaI, -FLOAT_1, FLOAT_1);
    // Potentially swap indices of refraction
    bool entering = cosThetaI > FLOAT_0;
    if (!entering)
    {
        std::swap(etaI, etaT);
        cosThetaI = std::abs(cosThetaI);
    }

    // Compute _cosThetaT_ using Snell's law
    Float sinThetaI = std::sqrt((std::max)(FLOAT_0, FLOAT_1 - cosThetaI * cosThetaI));
    Float sinThetaT = etaI / etaT * sinThetaI;

    // Handle total internal reflection
    if (sinThetaT >= FLOAT_1)
    {
        return FLOAT_1;
    }
    Float cosThetaT = std::sqrt((std::max)(FLOAT_0, FLOAT_1 - sinThetaT * sinThetaT));
    Float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
    Float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));

    return (Rparl * Rparl + Rperp * Rperp) / FLOAT_2;
}

// https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
color::Spectrum FrConductor(Float cosThetaI, const color::Spectrum &etai,
    const color::Spectrum &etat, const color::Spectrum &k)
{
    cosThetaI = common::math::Clamp(cosThetaI, -FLOAT_1, FLOAT_1);
    color::Spectrum eta = etat / etai;
    color::Spectrum etak = k / etai;

    Float cosThetaI2 = cosThetaI * cosThetaI;
    Float sinThetaI2 = FLOAT_1 - cosThetaI2;
    color::Spectrum eta2 = eta * eta;
    color::Spectrum etak2 = etak * etak;

    color::Spectrum t0 = eta2 - etak2 - sinThetaI2;
    color::Spectrum a2plusb2 = Sqrt(t0 * t0 + FLOAT_2 * FLOAT_2 * eta2 * etak2);
    color::Spectrum t1 = a2plusb2 + cosThetaI2;
    color::Spectrum a = Sqrt(FLOAT_INV_2 * (a2plusb2 + t0));
    color::Spectrum t2 = FLOAT_2 * cosThetaI * a;
    color::Spectrum Rs = (t1 - t2) / (t1 + t2);

    color::Spectrum t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
    color::Spectrum t4 = t2 * sinThetaI2;
    color::Spectrum Rp = Rs * (t3 - t4) / (t3 + t4);

    return FLOAT_INV_2 * (Rp + Rs);
}


}
}