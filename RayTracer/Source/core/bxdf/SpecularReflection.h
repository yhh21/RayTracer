#pragma once

#include "BxDF.h"
#include "Fresnel.h"

namespace core
{
namespace bxdf
{


class SpecularReflection : public BxDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    SpecularReflection(const color::Spectrum &R, Fresnel *fresnel)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR)),
        R(R),
        fresnel(fresnel)
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

    const color::Spectrum R;

    const Fresnel *fresnel;
};


}
}