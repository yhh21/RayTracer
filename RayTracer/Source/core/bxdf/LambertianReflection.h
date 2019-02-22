#pragma once

#include "BxDF.h"
#include "Fresnel.h"

namespace core
{
namespace bxdf
{


class LambertianReflection : public BxDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    LambertianReflection(const color::Spectrum &R)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R)
    {}


    color::Spectrum f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

    color::Spectrum rho(const common::math::Vec3f &, int, const common::math::Vec2f *) const
    {
        return R;
    }

    color::Spectrum rho(int, const common::math::Vec2f *, const common::math::Vec2f *) const
    {
        return R;
    }

private:

    const color::Spectrum R;
};


}
}