#pragma once

#include "../../ForwardDeclaration.h"
#include "../../common/math/Bounds3.h"

namespace core
{
namespace bvh
{


struct BVHBuildNode
{
    void InitLeaf(int first, int n, const common::math::Bounds3f &b)
    {
        firstPrimOffset = first;
        nPrimitives = n;
        bounds = b;
        p_left_child = p_right_child = nullptr;
        /*
        ++leafNodes;
        ++totalLeafNodes;
        totalPrimitives += n;
        */
    }

    void InitInterior(int axis, BVHBuildNode *_left_child, BVHBuildNode *_right_child)
    {
        p_left_child = _left_child;
        p_right_child = _right_child;
        bounds = Union(p_left_child->bounds, p_right_child->bounds);
        splitAxis = axis;
        nPrimitives = 0;
        /*
        ++interiorNodes;
        */
    }

    common::math::Bounds3f bounds;
    BVHBuildNode *p_left_child, *p_right_child;
    int splitAxis, firstPrimOffset, nPrimitives;
};


}
}