#include "MicrofacetDistribution.h"
#include "../BxDF.h"

namespace core
{
namespace bxdf
{
namespace distribution
{


MicrofacetDistribution::~MicrofacetDistribution()
{}

Float MicrofacetDistribution::Pdf(const common::math::Vec3f &wo,
    const common::math::Vec3f &wh) const
{
    if (sampleVisibleArea)
    {
        return D(wh) * G1(wo) * AbsDot(wo, wh) / AbsCosTheta(wo);
    }
    else
    {
        return D(wh) * AbsCosTheta(wh);
    }
}


}
}
}