#pragma once

#include "BxDF.h"
#include "Fresnel.h"
#include "../material/Material.h"

namespace core
{
namespace bxdf
{


class SpecularTransmission : public BxDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    SpecularTransmission(const color::Spectrum &T, Float etaA, Float etaB,
        material::TransportMode mode)
        : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR)),
        T(T),
        etaA(etaA),
        etaB(etaB),
        fresnel(etaA, etaB),
        mode(mode)
    {}


    color::Spectrum f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
    {
        return color::Spectrum(FLOAT_0);
    }

    color::Spectrum Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi, const common::math::Vec2f &sample,
        Float *pdf, BxDFType *sampledType) const;

    Float Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
    {
        return FLOAT_0;
    }

private:

    const color::Spectrum T;

    const Float etaA, etaB;

    const FresnelDielectric fresnel;

    const material::TransportMode mode;
};


}
}