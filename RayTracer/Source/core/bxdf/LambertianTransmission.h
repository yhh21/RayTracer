#pragma once

#include "../../ForwardDeclaration.h"
#include "BxDF.h"
#include "Fresnel.h"
#include "../color/Spectrum.h"

namespace core
{
namespace bxdf
{


class LambertianTransmission : public BxDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    LambertianTransmission(const color::Spectrum &T)
        : BxDF(BxDFType(BSDF_TRANSMISSION | BSDF_DIFFUSE)), T(T)
    {}


    color::Spectrum f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

    color::Spectrum rho(const common::math::Vec3f &, int, const common::math::Vec2f *) const
    {
        return T;
    }

    color::Spectrum rho(int, const common::math::Vec2f *, const common::math::Vec2f *) const
    {
        return T;
    }

    color::Spectrum Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi, const common::math::Vec2f &u,
        Float *pdf, BxDFType *sampledType) const;

    Float Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

private:

    color::Spectrum T;
};


}
}