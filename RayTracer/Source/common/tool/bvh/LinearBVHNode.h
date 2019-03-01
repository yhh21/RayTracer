#pragma once

#include "../../../ForwardDeclaration.h"
#include "../../math/Bounds3.h"

namespace common
{
namespace tool
{
namespace bvh
{


struct LinearBVHNode
{
    common::math::Bounds3f bounds;
    union
    {
        int primitivesOffset;   // leaf
        int secondChildOffset;  // interior
    };
    uint16_t nPrimitives;  // 0 -> interior node
    uint8_t axis;          // interior node: xyz
    uint8_t pad[1];        // ensure 32 byte total size
};


}
}
}