#pragma once

#include "BxDF.h"
#include "../color/Spectrum.h"

namespace core
{
namespace bxdf
{


class FresnelBlend : public BxDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    FresnelBlend(const core::color::Spectrum &Rd, const core::color::Spectrum &Rs,
        bxdf::distribution::MicrofacetDistribution *distrib);


    core::color::Spectrum f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

    core::color::Spectrum SchlickFresnel(Float cosTheta) const
    {
        auto pow5 = [](Float v)
        {
            return (v * v) * (v * v) * v;
        };
        return Rs + pow5(FLOAT_1 - cosTheta) * (core::color::Spectrum(FLOAT_1) - Rs);
    }

    core::color::Spectrum Sample_f(const common::math::Vec3f &wi, common::math::Vec3f *sampled_f
        , const common::math::Vec2f &u, Float *pdf, BxDFType *sampledType) const;

    Float Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

private:

    const core::color::Spectrum Rd, Rs;
    bxdf::distribution::MicrofacetDistribution *distribution;
};


}
}