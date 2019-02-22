#pragma once

#include "MicrofacetDistribution.h"
#include "../BxDF.h"

namespace core
{
namespace bxdf
{
namespace distribution
{


class BeckmannDistribution : public MicrofacetDistribution
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    BeckmannDistribution(Float alphax, Float alphay, bool samplevis = true)
        : MicrofacetDistribution(samplevis), alphax(alphax), alphay(alphay)
    {}


    static Float RoughnessToAlpha(Float roughness)
    {
        roughness = (std::max)(roughness, static_cast<Float>(1e-3));
        Float x = std::log(roughness);
        return static_cast<Float>(1.62142F)
            + static_cast<Float>(0.819955F) * x
            + static_cast<Float>(0.1734F) * x * x
            + static_cast<Float>(0.0171201F) * x * x * x
            + static_cast<Float>(0.000640711F) * x * x * x * x;
    }

    Float D(const common::math::Vec3f &wh) const;

    common::math::Vec3f Sample_wh(const common::math::Vec3f &wo, const common::math::Vec2f &u) const;

private:

    Float Lambda(const common::math::Vec3f &w) const;

    const Float alphax, alphay;
};


}
}
}