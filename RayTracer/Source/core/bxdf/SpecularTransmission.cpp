#include "SpecularTransmission.h"

namespace core
{
namespace bxdf
{


color::Spectrum SpecularTransmission::Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi,
    const common::math::Vec2f &sample, Float *pdf,
    BxDFType *sampledType) const
{
    // Figure out which $\eta$ is incident and which is transmitted
    bool entering = CosTheta(wo) > FLOAT_0;
    Float etaI = entering ? etaA : etaB;
    Float etaT = entering ? etaB : etaA;

    // Compute ray direction for specular transmission
    if (!Refract(wo, Faceforward(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_1), wo), etaI / etaT, wi))
    {
        return color::Spectrum(FLOAT_0);
    }
    *pdf = FLOAT_1;
    color::Spectrum ft = T * (color::Spectrum(FLOAT_1) - fresnel.Evaluate(CosTheta(*wi)));
    // Account for non-symmetry with transmission to different medium
    if (material::TransportMode::Radiance == mode)
    {
        ft *= (etaI * etaI) / (etaT * etaT);
    }

    return ft / AbsCosTheta(*wi);
}


}
}