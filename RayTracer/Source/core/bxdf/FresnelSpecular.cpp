#include "FresnelSpecular.h"
#include "Fresnel.h"

namespace core
{
namespace bxdf
{


color::Spectrum FresnelSpecular::Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi,
    const common::math::Vec2f &u, Float *pdf,
    BxDFType *sampledType) const
{
    Float F = FrDielectric(CosTheta(wo), etaA, etaB);
    if (u[0] < F)
    {
        // Compute specular reflection for _FresnelSpecular_

        // Compute perfect specular reflection direction
        *wi = common::math::Vec3f(-wo.x, -wo.y, wo.z);
        if (sampledType)
        {
            *sampledType = BxDFType(BSDF_SPECULAR | BSDF_REFLECTION);
        }
        *pdf = F;
        return F * R / AbsCosTheta(*wi);
    }
    else
    {
        // Compute specular transmission for _FresnelSpecular_

        // Figure out which $\eta$ is incident and which is transmitted
        bool entering = CosTheta(wo) > FLOAT_0;
        Float etaI = entering ? etaA : etaB;
        Float etaT = entering ? etaB : etaA;

        // Compute ray direction for specular transmission
        if (!Refract(wo, Faceforward(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_1), wo), etaI / etaT, wi))
        {
            return color::Spectrum(FLOAT_0);
        }
        color::Spectrum ft = T * (FLOAT_1 - F);

        // Account for non-symmetry with transmission to different medium
        if (material::TransportMode::Radiance == mode)
        {
            ft *= (etaI * etaI) / (etaT * etaT);
        }
        if (sampledType)
        {
            *sampledType = BxDFType(BSDF_SPECULAR | BSDF_TRANSMISSION);
        }
        *pdf = FLOAT_1 - F;
        return ft / AbsCosTheta(*wi);
    }
}


}
}