#include "BVHAccelerator.h"
#include "../MemoryArena.h"
#include "../MultiThread.h"
#include "../../math/Ray.h"
#include "../../math/Vec3.h"
#include "../../../core/primitive/Primitive.h"

namespace common
{
namespace tool
{
namespace bvh
{


struct BucketInfo
{
    int count = 0;
    common::math::Bounds3f bounds;
};

struct LBVHTreelet
{
    int startIndex, nPrimitives;
    BVHBuildNode *buildNodes;
};


inline
uint32_t LeftShift3(uint32_t x)
{
    CHECK_LE(x, (1 << 10));
    if ((1 << 10) == x)
    {
        --x;
    }

    x = (x | (x << 16)) & 0x30000ff;
    // x = ---- --98 ---- ---- ---- ---- 7654 3210
    x = (x | (x << 8)) & 0x300f00f;
    // x = ---- --98 ---- ---- 7654 ---- ---- 3210
    x = (x | (x << 4)) & 0x30c30c3;
    // x = ---- --98 ---- 76-- --54 ---- 32-- --10
    x = (x | (x << 2)) & 0x9249249;
    // x = ---- 9--8 --7- -6-- 5--4 --3- -2-- 1--0

    return x;
}

inline uint32_t EncodeMorton3(const common::math::Vec3f &v)
{
    CHECK_GE(v.x, FLOAT_0);
    CHECK_GE(v.y, FLOAT_0);
    CHECK_GE(v.z, FLOAT_0);
    return (LeftShift3(v.z) << 2) | (LeftShift3(v.y) << 1) | LeftShift3(v.x);
}

static void RadixSort(std::vector<MortonPrimitive> *v)
{
    std::vector<MortonPrimitive> tempVector(v->size());
    constexpr int bitsPerPass = 6;
    constexpr int nBits = 30;
    static_assert(0 == (nBits % bitsPerPass), "Radix sort bitsPerPass must evenly divide nBits");
    constexpr int nPasses = nBits / bitsPerPass;

    for (int pass = 0; pass < nPasses; ++pass)
    {
        // Perform one pass of radix sort, sorting _bitsPerPass_ bits
        int lowBit = pass * bitsPerPass;

        // Set in and out vector pointers for radix sort pass
        std::vector<MortonPrimitive> &in = (pass & 1) ? tempVector : *v;
        std::vector<MortonPrimitive> &out = (pass & 1) ? *v : tempVector;

        // Count number of zero bits in array for current radix sort bit
        constexpr int nBuckets = 1 << bitsPerPass;
        int bucketCount[nBuckets] = {0};
        constexpr int bitMask = (1 << bitsPerPass) - 1;
        for (const MortonPrimitive &mp : in)
        {
            int bucket = (mp.mortonCode >> lowBit) & bitMask;
            CHECK_GE(bucket, 0);
            CHECK_LT(bucket, nBuckets);
            ++bucketCount[bucket];
        }

        // Compute starting index in output array for each bucket
        int outIndex[nBuckets];
        outIndex[0] = 0;
        for (int i = 1; i < nBuckets; ++i)
        {
            outIndex[i] = outIndex[i - 1] + bucketCount[i - 1];
        }

        // Store sorted values in output array
        for (const MortonPrimitive &mp : in)
        {
            int bucket = (mp.mortonCode >> lowBit) & bitMask;
            out[outIndex[bucket]++] = mp;
        }
    }
    // Copy final result from _tempVector_, if needed
    if (nPasses & 1)
    {
        std::swap(*v, tempVector);
    }
}


BVHAccelerator::BVHAccelerator(std::vector<std::shared_ptr<core::primitive::Primitive>> p,
    int maxPrimsInNode, SplitMethod splitMethod)
    : maxPrimsInNode((std::min)(255, maxPrimsInNode)),
    splitMethod(splitMethod),
    primitives(std::move(p))
{
    //ProfilePhase _(Prof::AccelConstruction);
    if (primitives.empty())
    {
        return;
    }
    // Build BVH from _primitives_

    // Initialize _primitiveInfo_ array for primitives
    std::vector<BVHPrimitiveInfo> primitiveInfo(primitives.size());
    for (size_t i = 0; i < primitives.size(); ++i)
    {
        primitiveInfo[i] = {i, primitives[i]->WorldBound()};
    }

    // Build BVH tree for primitives using _primitiveInfo_
    common::tool::MemoryArena arena(1024 * 1024);
    int totalNodes = 0;
    std::vector<std::shared_ptr<core::primitive::Primitive>> orderedPrims;
    orderedPrims.reserve(primitives.size());
    BVHBuildNode *root;
    if (SplitMethod::HLBVH == splitMethod)
    {
        root = HLBVHBuild(arena, primitiveInfo, &totalNodes, orderedPrims);
    }
    else
    {
        root = recursiveBuild(arena, primitiveInfo, 0, primitives.size(),
            &totalNodes, orderedPrims);
    }
    primitives.swap(orderedPrims);
    primitiveInfo.resize(0);
    /*
    LOG(INFO) << StringPrintf("BVH created with %d nodes for %d "
        "primitives (%.2f MB), arena allocated %.2f MB",
        totalNodes, (int)primitives.size(),
        float(totalNodes * sizeof(LinearBVHNode)) /
        (1024.f * 1024.f),
        float(arena.TotalAllocated()) /
        (1024.f * 1024.f));
    */
    // Compute representation of depth-first traversal of BVH tree
    /*
    treeBytes += totalNodes * sizeof(LinearBVHNode) + sizeof(*this) +
        primitives.size() * sizeof(primitives[0]);
    */
    nodes = common::tool::AllocAligned<LinearBVHNode>(totalNodes);
    int offset = 0;
    flattenBVHTree(root, &offset);
    CHECK_EQ(totalNodes, offset);
}

BVHAccelerator::~BVHAccelerator()
{
    FreeAligned(nodes);
}


common::math::Bounds3f BVHAccelerator::WorldBound() const
{
    return nodes ? nodes[0].bounds : common::math::Bounds3f();
}

bool BVHAccelerator::Intersect(const common::math::Rayf &ray, core::interaction::SurfaceInteraction *isect) const
{
    if (!nodes)
    {
        return false;
    }
    //ProfilePhase p(Prof::AccelIntersect);
    bool hit = false;
    common::math::Vec3f invDir(FLOAT_1 / ray.dir.x, FLOAT_1 / ray.dir.y, FLOAT_1 / ray.dir.z);
    int dirIsNeg[3] = {invDir.x < FLOAT_0, invDir.y < FLOAT_0, invDir.z < FLOAT_0};
    // Follow ray through BVH nodes to find primitive intersections
    int toVisitOffset = 0, currentNodeIndex = 0;
    int nodesToVisit[64];
    while (true)
    {
        const LinearBVHNode *node = &nodes[currentNodeIndex];
        // Check ray against BVH node
        if (node->bounds.IntersectP(ray, invDir, dirIsNeg))
        {
            if (node->nPrimitives > 0)
            {
                // Intersect ray with primitives in leaf BVH node
                for (int i = 0; i < node->nPrimitives; ++i)
                {
                    if (primitives[node->primitivesOffset + i]->Intersect(
                        ray, isect))
                    {
                        hit = true;
                    }
                }
                if (0 == toVisitOffset)
                {
                    break;
                }
                currentNodeIndex = nodesToVisit[--toVisitOffset];
            }
            else
            {
                // Put far BVH node on _nodesToVisit_ stack, advance to near
                // node
                if (dirIsNeg[node->axis])
                {
                    nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                    currentNodeIndex = node->secondChildOffset;
                }
                else
                {
                    nodesToVisit[toVisitOffset++] = node->secondChildOffset;
                    currentNodeIndex = currentNodeIndex + 1;
                }
            }
        }
        else
        {
            if (0 == toVisitOffset)
            {
                break;
            }
            currentNodeIndex = nodesToVisit[--toVisitOffset];
        }
    }
    return hit;
}

bool BVHAccelerator::IntersectP(const common::math::Rayf &ray) const
{
    if (!nodes) return false;
    //ProfilePhase p(Prof::AccelIntersectP);
    common::math::Vec3f invDir(FLOAT_1 / ray.dir.x, FLOAT_1 / ray.dir.y, FLOAT_1 / ray.dir.z);
    int dirIsNeg[3] = {invDir.x < FLOAT_0, invDir.y < FLOAT_0, invDir.z < FLOAT_0};
    int nodesToVisit[64];
    int toVisitOffset = 0, currentNodeIndex = 0;
    while (true)
    {
        const LinearBVHNode *node = &nodes[currentNodeIndex];
        if (node->bounds.IntersectP(ray, invDir, dirIsNeg))
        {
            // Process BVH node _node_ for traversal
            if (node->nPrimitives > 0)
            {
                for (int i = 0; i < node->nPrimitives; ++i)
                {
                    if (primitives[node->primitivesOffset + i]->IntersectP(
                        ray))
                    {
                        return true;
                    }
                }
                if (0 == toVisitOffset)
                {
                    break;
                }
                currentNodeIndex = nodesToVisit[--toVisitOffset];
            }
            else
            {
                if (dirIsNeg[node->axis])
                {
                    /// second child first
                    nodesToVisit[toVisitOffset++] = currentNodeIndex + 1;
                    currentNodeIndex = node->secondChildOffset;
                }
                else
                {
                    nodesToVisit[toVisitOffset++] = node->secondChildOffset;
                    currentNodeIndex = currentNodeIndex + 1;
                }
            }
        }
        else
        {
            if (0 == toVisitOffset)
            {
                break;
            }
            currentNodeIndex = nodesToVisit[--toVisitOffset];
        }
    }
    return false;
}


BVHBuildNode *BVHAccelerator::recursiveBuild(
    MemoryArena &arena, std::vector<BVHPrimitiveInfo> &primitiveInfo, int start,
    int end, int *totalNodes,
    std::vector<std::shared_ptr<Primitive>> &orderedPrims)
{
    CHECK_NE(start, end);
    BVHBuildNode *node = arena.Alloc<BVHBuildNode>();
    (*totalNodes)++;
    // Compute bounds of all primitives in BVH node
    common::math::Bounds3f bounds;
    for (int i = start; i < end; ++i)
    {
        bounds = Union(bounds, primitiveInfo[i].bounds);
    }
    int nPrimitives = end - start;
    if (1 == nPrimitives)
    {
        // Create leaf _BVHBuildNode_
        int firstPrimOffset = orderedPrims.size();
        for (int i = start; i < end; ++i)
        {
            int primNum = primitiveInfo[i].primitiveNumber;
            orderedPrims.push_back(primitives[primNum]);
        }
        node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
        return node;
    }
    else
    {
        // Compute bound of primitive centroids, choose split dimension _dim_
        common::math::Bounds3f centroidBounds;
        for (int i = start; i < end; ++i)
        {
            centroidBounds = Union(centroidBounds, primitiveInfo[i].centroid);
        }
        int dim = centroidBounds.MaximumExtent();

        // Partition primitives into two sets and build children
        int mid = (start + end) / 2;
        if (centroidBounds.point_max[dim] == centroidBounds.point_min[dim])
        {
            // Create leaf _BVHBuildNode_
            int firstPrimOffset = orderedPrims.size();
            for (int i = start; i < end; ++i)
            {
                int primNum = primitiveInfo[i].primitiveNumber;
                orderedPrims.push_back(primitives[primNum]);
            }
            node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
            return node;
        }
        else
        {
            // Partition primitives based on _splitMethod_
            switch (splitMethod)
            {
            case SplitMethod::Middle:
            {
                // Partition primitives through node's midpoint
                Float pmid =
                    (centroidBounds.point_min[dim] + centroidBounds.point_max[dim]) / 2;
                BVHPrimitiveInfo *midPtr = std::partition(
                    &primitiveInfo[start], &primitiveInfo[end - 1] + 1,
                    [dim, pmid](const BVHPrimitiveInfo &pi)
                {
                    return pi.centroid[dim] < pmid;
                });
                mid = midPtr - &primitiveInfo[0];
                // For lots of prims with large overlapping bounding boxes, this
                // may fail to partition; in that case don't break and fall
                // through
                // to EqualCounts.
                if (mid != start && mid != end)
                {
                    break;
                }
            }
            case SplitMethod::EqualCounts:
            {
                // Partition primitives into equally-sized subsets
                mid = (start + end) / 2;
                std::nth_element(&primitiveInfo[start], &primitiveInfo[mid],
                    &primitiveInfo[end - 1] + 1,
                    [dim](const BVHPrimitiveInfo &a,
                        const BVHPrimitiveInfo &b)
                {
                    return a.centroid[dim] < b.centroid[dim];
                });
                break;
            }
            case SplitMethod::SAH:
            default:
            {
                // Partition primitives using approximate SAH
                if (nPrimitives <= 2)
                {
                    // Partition primitives into equally-sized subsets
                    mid = (start + end) / 2;
                    std::nth_element(&primitiveInfo[start], &primitiveInfo[mid],
                        &primitiveInfo[end - 1] + 1,
                        [dim](const BVHPrimitiveInfo &a,
                            const BVHPrimitiveInfo &b)
                    {
                        return a.centroid[dim] < b.centroid[dim];
                    });
                }
                else
                {
                    // Allocate _BucketInfo_ for SAH partition buckets
                    constexpr int nBuckets = 12;
                    BucketInfo buckets[nBuckets];

                    // Initialize _BucketInfo_ for SAH partition buckets
                    for (int i = start; i < end; ++i)
                    {
                        int b = nBuckets *
                            centroidBounds.Offset(
                                primitiveInfo[i].centroid)[dim];
                        if (b == nBuckets)
                        {
                            b = nBuckets - 1;
                        }
                        CHECK_GE(b, 0);
                        CHECK_LT(b, nBuckets);
                        buckets[b].count++;
                        buckets[b].bounds = Union(buckets[b].bounds, primitiveInfo[i].bounds);
                    }

                    // Compute costs for splitting after each bucket
                    Float cost[nBuckets - 1];
                    for (int i = 0; i < nBuckets - 1; ++i)
                    {
                        common::math::Bounds3f b0, b1;
                        int count0 = 0, count1 = 0;
                        for (int j = 0; j <= i; ++j)
                        {
                            b0 = Union(b0, buckets[j].bounds);
                            count0 += buckets[j].count;
                        }
                        for (int j = i + 1; j < nBuckets; ++j)
                        {
                            b1 = Union(b1, buckets[j].bounds);
                            count1 += buckets[j].count;
                        }
                        cost[i] = FLOAT_1 +
                            (count0 * b0.SurfaceArea() +
                                count1 * b1.SurfaceArea()) /
                            bounds.SurfaceArea();
                    }

                    // Find bucket to split at that minimizes SAH metric
                    Float minCost = cost[0];
                    int minCostSplitBucket = 0;
                    for (int i = 1; i < nBuckets - 1; ++i)
                    {
                        if (cost[i] < minCost)
                        {
                            minCost = cost[i];
                            minCostSplitBucket = i;
                        }
                    }

                    // Either create leaf or split primitives at selected SAH
                    // bucket
                    Float leafCost = nPrimitives;
                    if (nPrimitives > maxPrimsInNode || minCost < leafCost)
                    {
                        BVHPrimitiveInfo *pmid = std::partition(
                            &primitiveInfo[start], &primitiveInfo[end - 1] + 1,
                            [=](const BVHPrimitiveInfo &pi)
                        {
                            int b = nBuckets *
                                centroidBounds.Offset(pi.centroid)[dim];
                            if (b == nBuckets)
                            {
                                b = nBuckets - 1;
                            }
                            CHECK_GE(b, 0);
                            CHECK_LT(b, nBuckets);
                            return b <= minCostSplitBucket;
                        });
                        mid = pmid - &primitiveInfo[0];
                    }
                    else
                    {
                        // Create leaf _BVHBuildNode_
                        int firstPrimOffset = orderedPrims.size();
                        for (int i = start; i < end; ++i)
                        {
                            int primNum = primitiveInfo[i].primitiveNumber;
                            orderedPrims.push_back(primitives[primNum]);
                        }
                        node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
                        return node;
                    }
                }
                break;
            }
            }
            node->InitInterior(dim,
                recursiveBuild(arena, primitiveInfo, start, mid,
                    totalNodes, orderedPrims),
                recursiveBuild(arena, primitiveInfo, mid, end,
                    totalNodes, orderedPrims));
        }
    }
    return node;
}

BVHBuildNode *BVHAccelerator::HLBVHBuild(
    MemoryArena &arena, const std::vector<BVHPrimitiveInfo> &primitiveInfo,
    int *totalNodes,
    std::vector<std::shared_ptr<Primitive>> &orderedPrims) const
{
    // Compute bounding box of all primitive centroids
    common::math::Bounds3f bounds;
    for (const BVHPrimitiveInfo &pi : primitiveInfo)
    {
        bounds = Union(bounds, pi.centroid);
    }

    // Compute Morton indices of primitives
    std::vector<MortonPrimitive> mortonPrims(primitiveInfo.size());
    common::tool::ParallelFor([&](int i)
    {
        // Initialize _mortonPrims[i]_ for _i_th primitive
        constexpr int mortonBits = 10;
        constexpr int mortonScale = 1 << mortonBits;
        mortonPrims[i].primitiveIndex = primitiveInfo[i].primitiveNumber;
        common::math::Vec3f centroidOffset = bounds.Offset(primitiveInfo[i].centroid);
        mortonPrims[i].mortonCode = EncodeMorton3(centroidOffset * static_cast<Float>(mortonScale));
    }, primitiveInfo.size(), 512);

    // Radix sort primitive Morton indices
    RadixSort(&mortonPrims);

    // Create LBVH treelets at bottom of BVH

    // Find intervals of primitives for each treelet
    std::vector<LBVHTreelet> treeletsToBuild;
    for (int start = 0, end = 1; end <= (int)mortonPrims.size(); ++end)
    {
    #ifdef PBRT_HAVE_BINARY_CONSTANTS
        uint32_t mask = 0b00111111111111000000000000000000;
    #else
        uint32_t mask = 0x3ffc0000;
    #endif
        if ((int)mortonPrims.size() == end ||
            ((mortonPrims[start].mortonCode & mask) !=
            (mortonPrims[end].mortonCode & mask)))
        {
            // Add entry to _treeletsToBuild_ for this treelet
            int nPrimitives = end - start;
            int maxBVHNodes = 2 * nPrimitives;
            BVHBuildNode *nodes = arena.Alloc<BVHBuildNode>(maxBVHNodes, false);
            treeletsToBuild.push_back({start, nPrimitives, nodes});
            start = end;
        }
    }

    // Create LBVHs for treelets in parallel
    std::atomic<int> atomicTotal(0), orderedPrimsOffset(0);
    orderedPrims.resize(primitives.size());
    common::tool::ParallelFor([&](int i)
    {
        // Generate _i_th LBVH treelet
        int nodesCreated = 0;
        const int firstBitIndex = 29 - 12;
        LBVHTreelet &tr = treeletsToBuild[i];
        tr.buildNodes =
            emitLBVH(tr.buildNodes, primitiveInfo, &mortonPrims[tr.startIndex],
                tr.nPrimitives, &nodesCreated, orderedPrims,
                &orderedPrimsOffset, firstBitIndex);
        atomicTotal += nodesCreated;
    }, treeletsToBuild.size());
    *totalNodes = atomicTotal;

    // Create and return SAH BVH from LBVH treelets
    std::vector<BVHBuildNode *> finishedTreelets;
    finishedTreelets.reserve(treeletsToBuild.size());
    for (LBVHTreelet &treelet : treeletsToBuild)
    {
        finishedTreelets.push_back(treelet.buildNodes);
    }
    return buildUpperSAH(arena, finishedTreelets, 0, finishedTreelets.size(),
        totalNodes);
}

BVHBuildNode *BVHAccelerator::emitLBVH(
    BVHBuildNode *&buildNodes,
    const std::vector<BVHPrimitiveInfo> &primitiveInfo,
    MortonPrimitive *mortonPrims, int nPrimitives, int *totalNodes,
    std::vector<std::shared_ptr<Primitive>> &orderedPrims,
    std::atomic<int> *orderedPrimsOffset, int bitIndex) const
{
    CHECK_GT(nPrimitives, 0);
    if (-1 == bitIndex || nPrimitives < maxPrimsInNode)
    {
        // Create and return leaf node of LBVH treelet
        (*totalNodes)++;
        BVHBuildNode *node = buildNodes++;
        common::math::Bounds3f bounds;
        int firstPrimOffset = orderedPrimsOffset->fetch_add(nPrimitives);
        for (int i = 0; i < nPrimitives; ++i)
        {
            int primitiveIndex = mortonPrims[i].primitiveIndex;
            orderedPrims[firstPrimOffset + i] = primitives[primitiveIndex];
            bounds = Union(bounds, primitiveInfo[primitiveIndex].bounds);
        }
        node->InitLeaf(firstPrimOffset, nPrimitives, bounds);
        return node;
    }
    else
    {
        int mask = 1 << bitIndex;
        // Advance to next subtree level if there's no LBVH split for this bit
        if ((mortonPrims[0].mortonCode & mask) ==
            (mortonPrims[nPrimitives - 1].mortonCode & mask))
        {
            return emitLBVH(buildNodes, primitiveInfo, mortonPrims, nPrimitives,
                totalNodes, orderedPrims, orderedPrimsOffset,
                bitIndex - 1);
        }

        // Find LBVH split point for this dimension
        int searchStart = 0, searchEnd = nPrimitives - 1;
        while (searchStart + 1 != searchEnd)
        {
            CHECK_NE(searchStart, searchEnd);
            int mid = (searchStart + searchEnd) / 2;
            if ((mortonPrims[searchStart].mortonCode & mask) ==
                (mortonPrims[mid].mortonCode & mask))
            {
                searchStart = mid;
            }
            else
            {
                CHECK_EQ(mortonPrims[mid].mortonCode & mask,
                    mortonPrims[searchEnd].mortonCode & mask);
                searchEnd = mid;
            }
        }
        int splitOffset = searchEnd;
        CHECK_LE(splitOffset, nPrimitives - 1);
        CHECK_NE(mortonPrims[splitOffset - 1].mortonCode & mask,
            mortonPrims[splitOffset].mortonCode & mask);

        // Create and return interior LBVH node
        (*totalNodes)++;
        BVHBuildNode *node = buildNodes++;
        BVHBuildNode *lbvh[2] = {
            emitLBVH(buildNodes, primitiveInfo, mortonPrims, splitOffset,
            totalNodes, orderedPrims, orderedPrimsOffset,
            bitIndex - 1),
            emitLBVH(buildNodes, primitiveInfo, &mortonPrims[splitOffset],
            nPrimitives - splitOffset, totalNodes, orderedPrims,
            orderedPrimsOffset, bitIndex - 1)};
        int axis = bitIndex % 3;
        node->InitInterior(axis, lbvh[0], lbvh[1]);
        return node;
    }
}

BVHBuildNode *BVHAccelerator::buildUpperSAH(MemoryArena &arena,
    std::vector<BVHBuildNode *> &treeletRoots,
    int start, int end,
    int *totalNodes) const
{
    CHECK_LT(start, end);
    int nNodes = end - start;
    if (1 == nNodes)
    {
        return treeletRoots[start];
    }
    ++(*totalNodes);
    BVHBuildNode *node = arena.Alloc<BVHBuildNode>();

    // Compute bounds of all nodes under this HLBVH node
    common::math::Bounds3f bounds;
    for (int i = start; i < end; ++i)
        bounds = Union(bounds, treeletRoots[i]->bounds);

    // Compute bound of HLBVH node centroids, choose split dimension _dim_
    common::math::Bounds3f centroidBounds;
    for (int i = start; i < end; ++i)
    {
        common::math::Vec3f centroid =
            (treeletRoots[i]->bounds.point_min + treeletRoots[i]->bounds.point_max) *
            FLOAT_INV_2;
        centroidBounds = Union(centroidBounds, centroid);
    }
    int dim = centroidBounds.MaximumExtent();
    // FIXME: if this hits, what do we need to do?
    // Make sure the SAH split below does something... ?
    CHECK_NE(centroidBounds.point_max[dim], centroidBounds.point_min[dim]);

    // Allocate _BucketInfo_ for SAH partition buckets
    constexpr int nBuckets = 12;
    struct BucketInfo
    {
        int count = 0;
        common::math::Bounds3f bounds;
    };
    BucketInfo buckets[nBuckets];

    // Initialize _BucketInfo_ for HLBVH SAH partition buckets
    for (int i = start; i < end; ++i)
    {
        Float centroid = (treeletRoots[i]->bounds.point_min[dim] +
            treeletRoots[i]->bounds.point_max[dim]) *
            FLOAT_INV_2;
        int b =
            nBuckets * ((centroid - centroidBounds.point_min[dim]) /
            (centroidBounds.point_max[dim] - centroidBounds.point_min[dim]));
        if (b == nBuckets)
        {
            b = nBuckets - 1;
        }
        CHECK_GE(b, 0);
        CHECK_LT(b, nBuckets);
        buckets[b].count++;
        buckets[b].bounds = Union(buckets[b].bounds, treeletRoots[i]->bounds);
    }

    // Compute costs for splitting after each bucket
    Float cost[nBuckets - 1];
    for (int i = 0; i < nBuckets - 1; ++i)
    {
        common::math::Bounds3f b0, b1;
        int count0 = 0, count1 = 0;
        for (int j = 0; j <= i; ++j)
        {
            b0 = Union(b0, buckets[j].bounds);
            count0 += buckets[j].count;
        }
        for (int j = i + 1; j < nBuckets; ++j)
        {
            b1 = Union(b1, buckets[j].bounds);
            count1 += buckets[j].count;
        }
        cost[i] = .125f +
            (count0 * b0.SurfaceArea() + count1 * b1.SurfaceArea()) /
            bounds.SurfaceArea();
    }

    // Find bucket to split at that minimizes SAH metric
    Float minCost = cost[0];
    int minCostSplitBucket = 0;
    for (int i = 1; i < nBuckets - 1; ++i)
    {
        if (cost[i] < minCost)
        {
            minCost = cost[i];
            minCostSplitBucket = i;
        }
    }

    // Split nodes and create interior HLBVH SAH node
    BVHBuildNode **pmid = std::partition(
        &treeletRoots[start], &treeletRoots[end - 1] + 1,
        [=](const BVHBuildNode *node)
    {
        Float centroid =
            (node->bounds.point_min[dim] + node->bounds.point_max[dim]) * FLOAT_INV_2;
        int b = nBuckets *
            ((centroid - centroidBounds.point_min[dim]) /
            (centroidBounds.point_max[dim] - centroidBounds.point_min[dim]));
        if (b == nBuckets)
        {
            b = nBuckets - 1;
        }
        CHECK_GE(b, 0);
        CHECK_LT(b, nBuckets);
        return b <= minCostSplitBucket;
    });
    int mid = pmid - &treeletRoots[0];
    CHECK_GT(mid, start);
    CHECK_LT(mid, end);
    node->InitInterior(
        dim, this->buildUpperSAH(arena, treeletRoots, start, mid, totalNodes),
        this->buildUpperSAH(arena, treeletRoots, mid, end, totalNodes));
    return node;
}

int BVHAccelerator::flattenBVHTree(BVHBuildNode *node, int *offset)
{
    LinearBVHNode *linearNode = &nodes[*offset];
    linearNode->bounds = node->bounds;
    int myOffset = (*offset)++;
    if (node->nPrimitives > 0)
    {
        CHECK(!node->children[0] && !node->children[1]);
        CHECK_LT(node->nPrimitives, 65536);
        linearNode->primitivesOffset = node->firstPrimOffset;
        linearNode->nPrimitives = node->nPrimitives;
    }
    else
    {
        // Create interior flattened BVH node
        linearNode->axis = node->splitAxis;
        linearNode->nPrimitives = 0;
        flattenBVHTree(node->children[0], offset);
        linearNode->secondChildOffset =
            flattenBVHTree(node->children[1], offset);
    }
    return myOffset;
}


}
}
}