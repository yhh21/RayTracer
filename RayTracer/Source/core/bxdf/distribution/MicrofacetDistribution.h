#pragma once

#include "../../../ForwardDeclaration.h"
#include "../../../common/math/Vec2.h"
#include "../../../common/math/Vec3.h"
#include "../../../common/math/Constants.h"

namespace core
{
namespace bxdf
{
namespace distribution
{


class MicrofacetDistribution
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    virtual ~MicrofacetDistribution();


    virtual Float D(const common::math::Vec3f &wh) const = 0;

    virtual Float Lambda(const common::math::Vec3f &w) const = 0;

    Float G1(const common::math::Vec3f &w) const
    {
        //    if (Dot(w, wh) * CosTheta(w) < 0.) return 0.;
        return FLOAT_1 / (FLOAT_1 + Lambda(w));
    }

    virtual Float G(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
    {
        return FLOAT_1 / (FLOAT_1 + Lambda(wo) + Lambda(wi));
    }

    virtual common::math::Vec3f Sample_wh(const common::math::Vec3f &wo, const common::math::Vec2f &u) const = 0;

    Float Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wh) const;

protected:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    MicrofacetDistribution(bool sampleVisibleArea)
        : sampleVisibleArea(sampleVisibleArea)
    {}

    // MicrofacetDistribution Protected Data
    const bool sampleVisibleArea;
};


}
}
}