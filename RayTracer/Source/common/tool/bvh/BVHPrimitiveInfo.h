#pragma once

#include "../../../ForwardDeclaration.h"
#include "../../math/Bounds3.h"

namespace common
{
namespace tool
{
namespace bvh
{


struct BVHPrimitiveInfo
{
    size_t primitiveNumber;

    common::math::Bounds3f bounds;

    common::math::Vec3f centroid;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    BVHPrimitiveInfo()
    {}

    BVHPrimitiveInfo(size_t primitiveNumber, const common::math::Bounds3f &bounds)
        : primitiveNumber(primitiveNumber),
        bounds(bounds),
        centroid(FLOAT_INV_2 * bounds.point_min + FLOAT_INV_2 * bounds.point_max)
    {}
};


}
}
}