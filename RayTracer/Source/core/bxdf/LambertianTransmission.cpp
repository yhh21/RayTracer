#include "LambertianTransmission.h"
#include "../../common/math/Constants.h"
#include "../sampler/Sampling.h"

namespace core
{
namespace bxdf
{


color::Spectrum LambertianTransmission::f(const common::math::Vec3f &wo,
    const common::math::Vec3f &wi) const
{
    return T * common::math::INV_PI;
}

color::Spectrum LambertianTransmission::Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi,
    const common::math::Vec2f &u, Float *pdf,
    BxDFType *sampledType) const
{
    *wi = sampler::CosineSampleHemisphere(u);
    if (wo.z > FLOAT_0)
    {
        wi->z *= -FLOAT_1;
    }
    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}

Float LambertianTransmission::Pdf(const common::math::Vec3f &wo,
    const common::math::Vec3f &wi) const
{
    return !SameHemisphere(wo, wi) ? AbsCosTheta(wi) * common::math::INV_PI : FLOAT_0;
}

}
}