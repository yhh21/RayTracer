#pragma once

namespace common
{
namespace tool
{
namespace bvh
{


struct MortonPrimitive
{
    int primitiveIndex;
    uint32_t mortonCode;
};


}
}
}