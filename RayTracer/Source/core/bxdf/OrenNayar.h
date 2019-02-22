#pragma once

#include "BxDF.h"
#include "Fresnel.h"
#include "../../common/math/Constants.h"

namespace core
{
namespace bxdf
{


class OrenNayar : public BxDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    OrenNayar(const core::color::Spectrum &R, Float sigma)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), R(R)
    {
        sigma = common::math::ConvertToRadians(sigma);
        Float sigma2 = sigma * sigma;
        A = FLOAT_1 - (sigma2 / (FLOAT_2 * (sigma2 + static_cast<Float>(0.33F))));
        B = static_cast<Float>(0.45F) * sigma2 / (sigma2 + static_cast<Float>(0.09F));
    }


    core::color::Spectrum f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

private:

    const core::color::Spectrum R;

    Float A, B;
};


}
}