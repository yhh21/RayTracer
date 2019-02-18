#pragma once

#include "../../ForwardDeclaration.h"
#include "BxDF.h"
#include "Fresnel.h"
#include "../color/Spectrum.h"
#include "../material/Material.h"

namespace core
{
namespace bxdf
{


class FresnelSpecular : public BxDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    FresnelSpecular(const color::Spectrum &R, const color::Spectrum &T, Float etaA,
        Float etaB, material::TransportMode mode)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_SPECULAR)),
        R(R),
        T(T),
        etaA(etaA),
        etaB(etaB),
        mode(mode)
    {}


    color::Spectrum f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
    {
        return color::Spectrum(FLOAT_0);
    }

    color::Spectrum Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi, const common::math::Vec2f &u,
        Float *pdf, BxDFType *sampledType) const;

    Float Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
    {
        return FLOAT_0;
    }

private:

    const color::Spectrum R, T;

    const Float etaA, etaB;

    const material::TransportMode mode;
};


}
}