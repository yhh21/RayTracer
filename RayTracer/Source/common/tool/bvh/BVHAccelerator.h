#pragma once

#include "../../../core/primitive/Aggregate.h"
#include "BVHBuildNode.h"
#include "BVHPrimitiveInfo.h"
#include "MortonPrimitive.h"
#include "LinearBVHNode.h"
#include <atomic>

namespace common
{
namespace tool
{
namespace bvh
{


class BVHAccelerator : public core::primitive::Aggregate
{
public:
    enum class SplitMethod
    {
        SAH,
        HLBVH,
        Middle,
        EqualCounts
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    BVHAccelerator(std::vector<std::shared_ptr<core::primitive::Primitive>> p
        , int maxPrimsInNode = 1, SplitMethod splitMethod = SplitMethod::SAH);

    ~BVHAccelerator();


    common::math::Bounds3f WorldBound() const;

    bool Intersect(const common::math::Rayf &ray, core::interaction::SurfaceInteraction *isect) const;

    bool IntersectP(const common::math::Rayf &ray) const;

private:

    BVHBuildNode *recursiveBuild(
        common::tool::MemoryArena &arena, std::vector<BVHPrimitiveInfo> &primitiveInfo,
        int start, int end, int *totalNodes,
        std::vector<std::shared_ptr<core::primitive::Primitive>> &orderedPrims);

    BVHBuildNode *HLBVHBuild(
        common::tool::MemoryArena &arena, const std::vector<BVHPrimitiveInfo> &primitiveInfo,
        int *totalNodes,
        std::vector<std::shared_ptr<core::primitive::Primitive>> &orderedPrims) const;

    BVHBuildNode *emitLBVH(
        BVHBuildNode *&buildNodes,
        const std::vector<BVHPrimitiveInfo> &primitiveInfo,
        MortonPrimitive *mortonPrims, int nPrimitives, int *totalNodes,
        std::vector<std::shared_ptr<core::primitive::Primitive>> &orderedPrims,
        std::atomic<int> *orderedPrimsOffset, int bitIndex) const;

    BVHBuildNode *buildUpperSAH(common::tool::MemoryArena &arena,
        std::vector<BVHBuildNode *> &treeletRoots,
        int start, int end, int *totalNodes) const;

    int flattenBVHTree(BVHBuildNode *node, int *offset);


    const int maxPrimsInNode;
    const SplitMethod splitMethod;
    std::vector<std::shared_ptr<core::primitive::Primitive>> primitives;
    LinearBVHNode *nodes = nullptr;
};


}
}
}