#pragma once

#include "../../../core/primitive/Aggregate.h"
#include "BoundEdge.h"
#include "KDTreeNode.h"
#include "../../math/Bounds3.h"
#include <vector>

namespace common
{
namespace tool
{
namespace kdtree
{


class KDTreeAccelerator : public core::primitive::Aggregate
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    KDTreeAccelerator(std::vector<std::shared_ptr<core::primitive::Primitive>> p,
        int isectCost = 80, int traversalCost = 1,
        Float emptyBonus = FLOAT_INV_2, int maxPrims = 1, int maxDepth = -1);

    ~KDTreeAccelerator();


    common::math::Bounds3f WorldBound() const
    {
        return bounds;
    }

    bool Intersect(const common::math::Rayf &ray, core::interaction::SurfaceInteraction *isect) const;

    bool IntersectP(const common::math::Rayf &ray) const;

private:

    void buildTree(int nodeNum, const common::math::Bounds3f &bounds,
        const std::vector<common::math::Bounds3f> &primBounds, int *primNums,
        int nprims, int depth,
        const std::unique_ptr<BoundEdge[]> edges[3], int *prims0,
        int *prims1, int badRefines = 0);


    const int isectCost, traversalCost, maxPrims;

    const Float emptyBonus;

    std::vector<std::shared_ptr<core::primitive::Primitive>> primitives;

    std::vector<int> primitiveIndices;

    KDTreeNode *nodes;

    int nAllocedNodes, nextFreeNode;

    common::math::Bounds3f bounds;
};


}
}
}