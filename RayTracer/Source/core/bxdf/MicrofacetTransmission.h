#pragma once

#include "BxDF.h"
#include "Fresnel.h"
#include "./distribution/MicrofacetDistribution.h"
#include "../material/Material.h"

namespace core
{
namespace bxdf
{


class MicrofacetTransmission : public BxDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    MicrofacetTransmission(const core::color::Spectrum &T,
        core::bxdf::distribution::MicrofacetDistribution *distribution, Float etaA,
        Float etaB, core::material::TransportMode mode)
        : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_GLOSSY)),
        T(T),
        distribution(distribution),
        etaA(etaA),
        etaB(etaB),
        fresnel(etaA, etaB),
        mode(mode)
    {}


    core::color::Spectrum f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

    core::color::Spectrum Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi
        , const common::math::Vec2f &u, Float *pdf, BxDFType *sampledType) const;

    Float Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

private:

    const core::color::Spectrum T;

    const core::bxdf::distribution::MicrofacetDistribution *distribution;

    const Float etaA, etaB;

    const FresnelDielectric fresnel;

    const core::material::TransportMode mode;
};


}
}