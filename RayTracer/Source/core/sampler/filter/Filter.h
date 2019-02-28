#pragma once

#include "../../../ForwardDeclaration.h"
#include "../../../common/math/Vec2.h"

namespace core
{
namespace sampler
{
namespace filter
{


typedef enum
{
    FILTER_NONE
    , FILTER_BOX
    , FILTER_GAUSSIAN
    , FILTER_MITCHELL
    , FILTER_MITCHELL_SS
    , FILTER_BLACKMANHARRIS
    , FILTER_TYPE_COUNT
} FilterType;

class Filter
{
public:

    const common::math::Vec2f radius, invRadius;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    Filter(const common::math::Vec2f &radius)
        : radius(radius),
        invRadius(common::math::Vec2f(FLOAT_1 / radius.x, FLOAT_1 / radius.y))
    {}

    virtual ~Filter()
    {}


    virtual Float Evaluate(const common::math::Vec2f &p) const = 0;
};


}
}
}