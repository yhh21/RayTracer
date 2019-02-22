#pragma once

#include "BxDF.h"
#include "Fresnel.h"

namespace core
{
namespace bxdf
{


class ScaledBxDF : public BxDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    ScaledBxDF(BxDF *bxdf, const core::color::Spectrum &scale)
        : BxDF(BxDFType(bxdf->type)), bxdf(bxdf), scale(scale)
    {}


    core::color::Spectrum rho(const common::math::Vec3f &w, int nSamples,
        const common::math::Vec2f *samples) const
    {
        return scale * bxdf->rho(w, nSamples, samples);
    }

    core::color::Spectrum rho(int nSamples, const common::math::Vec2f *samples1,
        const common::math::Vec2f *samples2) const
    {
        return scale * bxdf->rho(nSamples, samples1, samples2);
    }

    core::color::Spectrum f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

    core::color::Spectrum Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi,
        const common::math::Vec2f &sample,
        Float *pdf, BxDFType *sampledType) const;

    Float Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

private:

    BxDF *bxdf;

    core::color::Spectrum scale;
};


}
}