#pragma once

#include "BxDF.h"
#include "Fresnel.h"
#include "./distribution/MicrofacetDistribution.h"

namespace core
{
namespace bxdf
{


class MicrofacetReflection : public BxDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    MicrofacetReflection(const core::color::Spectrum &R,
        core::bxdf::distribution::MicrofacetDistribution *distribution, Fresnel *fresnel)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY)),
        R(R),
        distribution(distribution),
        fresnel(fresnel)
    {}


    core::color::Spectrum f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

    core::color::Spectrum Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi
        , const common::math::Vec2f &u, Float *pdf, BxDFType *sampledType) const;

    Float Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

private:

    const core::color::Spectrum R;

    const core::bxdf::distribution::MicrofacetDistribution *distribution;

    const Fresnel *fresnel;
};


}
}