#include "ScaledBxDF.h"
#include "../../common/math/Constants.h"
#include "../sampler/Sampling.h"

namespace core
{
namespace bxdf
{


core::color::Spectrum ScaledBxDF::f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
{
    return scale * bxdf->f(wo, wi);
}

core::color::Spectrum ScaledBxDF::Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi,
    const common::math::Vec2f &sample, Float *pdf,
    BxDFType *sampledType) const
{
    core::color::Spectrum f = bxdf->Sample_f(wo, wi, sample, pdf, sampledType);
    return scale * f;
}

Float ScaledBxDF::Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
{
    return bxdf->Pdf(wo, wi);
}


}
}