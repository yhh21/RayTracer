#include "OrenNayar.h"
#include "../sampler/Sampling.h"

namespace core
{
namespace bxdf
{


core::color::Spectrum OrenNayar::f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
{
    Float sinThetaI = SinTheta(wi);
    Float sinThetaO = SinTheta(wo);
    // Compute cosine term of Oren-Nayar model
    Float maxCos = FLOAT_0;
    if (sinThetaI > 1e-4 && sinThetaO > 1e-4)
    {
        Float sinPhiI = SinPhi(wi), cosPhiI = CosPhi(wi);
        Float sinPhiO = SinPhi(wo), cosPhiO = CosPhi(wo);
        Float dCos = cosPhiI * cosPhiO + sinPhiI * sinPhiO;
        maxCos = (std::max)(FLOAT_0, dCos);
    }

    // Compute sine and tangent terms of Oren-Nayar model
    Float sinAlpha, tanBeta;
    if (AbsCosTheta(wi) > AbsCosTheta(wo))
    {
        sinAlpha = sinThetaO;
        tanBeta = sinThetaI / AbsCosTheta(wi);
    }
    else
    {
        sinAlpha = sinThetaI;
        tanBeta = sinThetaO / AbsCosTheta(wo);
    }

    return R * common::math::INV_PI * (A + B * maxCos * sinAlpha * tanBeta);
}


}
}