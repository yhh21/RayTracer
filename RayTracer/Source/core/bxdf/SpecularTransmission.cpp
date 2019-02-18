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
    bool entering = CosTheta(wo) > 0;
    Float etaI = entering ? etaA : etaB;
    Float etaT = entering ? etaB : etaA;

    // Compute ray direction for specular transmission
    if (!Refract(wo, Faceforward(common::math::Vec3f(0, 0, 1), wo), etaI / etaT, wi))
        return 0;
    *pdf = 1;
    color::Spectrum ft = T * (color::Spectrum(1.) - fresnel.Evaluate(CosTheta(*wi)));
    // Account for non-symmetry with transmission to different medium
    if (mode == material::TransportMode::Radiance) ft *= (etaI * etaI) / (etaT * etaT);
    return ft / AbsCosTheta(*wi);
}


}
}