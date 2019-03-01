#pragma once

#include "../../../ForwardDeclaration.h"
#include "../../math/Bounds3.h"

namespace common
{
namespace tool
{
namespace bvh
{


struct BVHBuildNode
{
    common::math::Bounds3f bounds;

    BVHBuildNode *children[2];

    int splitAxis, firstPrimOffset, nPrimitives;


    void InitLeaf(int first, int n, const common::math::Bounds3f &b)
    {
        firstPrimOffset = first;
        nPrimitives = n;
        bounds = b;
        children[0] = children[1] = nullptr;
        /*
        ++leafNodes;
        ++totalLeafNodes;
        totalPrimitives += n;
        */
    }

    void InitInterior(int axis, BVHBuildNode *p_left_child, BVHBuildNode *p_right_child)
    {
        children[0] = p_left_child;
        children[1] = p_right_child;
        bounds = Union(p_left_child->bounds, p_right_child->bounds);
        splitAxis = axis;
        nPrimitives = 0;
        /*
        ++interiorNodes;
        */
    }
};


}
}
}