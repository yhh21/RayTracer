#include "SpecularReflection.h"


namespace core
{
namespace bxdf
{


color::Spectrum SpecularReflection::Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi,
    const common::math::Vec2f &sample, Float *pdf,
    BxDFType *sampledType) const
{
    // Compute perfect specular reflection direction
    *wi = common::math::Vec3f(-wo.x, -wo.y, wo.z);
    *pdf = 1;
    return fresnel->Evaluate(CosTheta(*wi)) * R / AbsCosTheta(*wi);
}


}
}