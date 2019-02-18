#include "LambertianReflection.h"


namespace core
{
namespace bxdf
{


color::Spectrum LambertianReflection::f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
{
    return R * common::math::INV_PI;
}


}
}