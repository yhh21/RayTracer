#include "KDTreeAccelerator.h"
#include "../MemoryArena.h"
#include "../MultiThread.h"
#include "../../math/Ray.h"
#include "../../math/Vec3.h"
#include "../../../core/primitive/Primitive.h"

namespace common
{
namespace tool
{
namespace kdtree
{


struct KDTreeToDo
{
    const KDTreeNode *node;
    Float t_min, t_max;
};


KDTreeAccelerator::KDTreeAccelerator(std::vector<std::shared_ptr<core::primitive::Primitive>> p,
    int isectCost, int traversalCost, Float emptyBonus,
    int maxPrims, int maxDepth)
    : isectCost(isectCost),
    traversalCost(traversalCost),
    maxPrims(maxPrims),
    emptyBonus(emptyBonus),
    primitives(std::move(p))
{
    // Build kd-tree for accelerator
    //ProfilePhase _(Prof::AccelConstruction);
    nextFreeNode = nAllocedNodes = 0;
    if (maxDepth <= 0)
    {
        maxDepth = static_cast<int>(std::round(static_cast<Float>(8.0F)
            + static_cast<Float>(1.3F) * common::math::Log2Int(int64_t(primitives.size()))));
    }

    // Compute bounds for kd-tree construction
    std::vector<common::math::Bounds3f> primBounds;
    primBounds.reserve(primitives.size());
    for (const std::shared_ptr<core::primitive::Primitive> &prim : primitives)
    {
        common::math::Bounds3f b = prim->WorldBound();
        bounds = Union(bounds, b);
        primBounds.push_back(b);
    }

    // Allocate working memory for kd-tree construction
    std::unique_ptr<BoundEdge[]> edges[3];
    for (int i = 0; i < 3; ++i)
    {
        edges[i].reset(new BoundEdge[2 * primitives.size()]);
    }
    std::unique_ptr<int[]> prims0(new int[primitives.size()]);
    std::unique_ptr<int[]> prims1(new int[(maxDepth + 1) * primitives.size()]);

    // Initialize _primNums_ for kd-tree construction
    std::unique_ptr<int[]> primNums(new int[primitives.size()]);
    for (size_t i = 0; i < primitives.size(); ++i)
    {
        primNums[i] = i;
    }

    // Start recursive construction of kd-tree
    buildTree(0, bounds, primBounds, primNums.get(), primitives.size(),
        maxDepth, edges, prims0.get(), prims1.get());
}

KDTreeAccelerator::~KDTreeAccelerator()
{
    FreeAligned(nodes);
}


void KDTreeAccelerator::buildTree(int nodeNum, const common::math::Bounds3f &nodeBounds,
    const std::vector<common::math::Bounds3f> &allPrimBounds,
    int *primNums, int nPrimitives, int depth,
    const std::unique_ptr<BoundEdge[]> edges[3],
    int *prims0, int *prims1, int badRefines)
{
    CHECK_EQ(nodeNum, nextFreeNode);
    // Get next free node from _nodes_ array
    if (nextFreeNode == nAllocedNodes)
    {
        int nNewAllocNodes = (std::max)(2 * nAllocedNodes, 512);
        KDTreeNode *n = AllocAligned<KDTreeNode>(nNewAllocNodes);
        if (nAllocedNodes > 0)
        {
            memcpy(n, nodes, nAllocedNodes * sizeof(KDTreeNode));
            FreeAligned(nodes);
        }
        nodes = n;
        nAllocedNodes = nNewAllocNodes;
    }
    ++nextFreeNode;

    // Initialize leaf node if termination criteria met
    if (nPrimitives <= maxPrims || 0 == depth)
    {
        nodes[nodeNum].InitLeaf(primNums, nPrimitives, &primitiveIndices);
        return;
    }

    // Initialize interior node and continue recursion

    // Choose split axis position for interior node
    int bestAxis = -1, bestOffset = -1;
    Float bestCost = (std::numeric_limits<Float>::max)();
    Float oldCost = isectCost * static_cast<Float>(nPrimitives);
    Float totalSA = nodeBounds.SurfaceArea();
    Float invTotalSA = FLOAT_1 / totalSA;
    common::math::Vec3f d = nodeBounds.point_max - nodeBounds.point_min;

    // Choose which axis to split along
    int axis = nodeBounds.MaximumExtent();
    int retries = 0;
retrySplit:

    // Initialize edges for _axis_
    for (int i = 0; i < nPrimitives; ++i)
    {
        int pn = primNums[i];
        const common::math::Bounds3f &bounds = allPrimBounds[pn];
        edges[axis][2 * i] = BoundEdge(bounds.point_min[axis], pn, true);
        edges[axis][2 * i + 1] = BoundEdge(bounds.point_max[axis], pn, false);
    }

    // Sort _edges_ for _axis_
    std::sort(&edges[axis][0], &edges[axis][2 * nPrimitives],
        [](const BoundEdge &e0, const BoundEdge &e1) -> bool
    {
        if (e0.t == e1.t)
        {
            return (int)e0.type < (int)e1.type;
        }
        else
        {
            return e0.t < e1.t;
        }
    });

    // Compute cost of all splits for _axis_ to find best
    int nBelow = 0, nAbove = nPrimitives;
    for (int i = 0; i < 2 * nPrimitives; ++i)
    {
        if (EdgeType::End == edges[axis][i].type)
        {
            --nAbove;
        }
        Float edgeT = edges[axis][i].t;
        if (edgeT > nodeBounds.point_min[axis] && edgeT < nodeBounds.point_max[axis])
        {
            // Compute cost for split at _i_th edge

            // Compute child surface areas for split at _edgeT_
            int otherAxis0 = (axis + 1) % 3, otherAxis1 = (axis + 2) % 3;
            Float belowSA = FLOAT_2 * (d[otherAxis0] * d[otherAxis1] +
                (edgeT - nodeBounds.point_min[axis]) *
                (d[otherAxis0] + d[otherAxis1]));
            Float aboveSA = FLOAT_2 * (d[otherAxis0] * d[otherAxis1] +
                (nodeBounds.point_max[axis] - edgeT) *
                (d[otherAxis0] + d[otherAxis1]));
            Float pBelow = belowSA * invTotalSA;
            Float pAbove = aboveSA * invTotalSA;
            Float eb = (0 == nAbove || 0 == nBelow) ? emptyBonus : 0;
            Float cost = traversalCost +
                isectCost * (FLOAT_1 - eb) * (pBelow * nBelow + pAbove * nAbove);

            // Update best split if this is lowest cost so far
            if (cost < bestCost)
            {
                bestCost = cost;
                bestAxis = axis;
                bestOffset = i;
            }
        }
        if (EdgeType::Start == edges[axis][i].type)
        {
            ++nBelow;
        }
    }
    CHECK(nBelow == nPrimitives && 0 == nAbove);

    // Create leaf if no good splits were found
    if (-1 == bestAxis && retries < 2)
    {
        ++retries;
        axis = (axis + 1) % 3;
        goto retrySplit;
    }
    if (bestCost > oldCost)
    {
        ++badRefines;
    }
    if ((bestCost > static_cast<Float>(4.0F) * oldCost && nPrimitives < 16) || -1 == bestAxis ||
        3 == badRefines)
    {
        nodes[nodeNum].InitLeaf(primNums, nPrimitives, &primitiveIndices);
        return;
    }

    // Classify primitives with respect to split
    int n0 = 0, n1 = 0;
    for (int i = 0; i < bestOffset; ++i)
    {
        if (EdgeType::Start == edges[bestAxis][i].type)
        {
            prims0[n0++] = edges[bestAxis][i].primNum;
        }
    }
    for (int i = bestOffset + 1; i < 2 * nPrimitives; ++i)
    {
        if (EdgeType::End == edges[bestAxis][i].type)
        {
            prims1[n1++] = edges[bestAxis][i].primNum;
        }
    }

    // Recursively initialize children nodes
    Float tSplit = edges[bestAxis][bestOffset].t;
    common::math::Bounds3f bounds0 = nodeBounds, bounds1 = nodeBounds;
    bounds0.point_max[bestAxis] = bounds1.point_min[bestAxis] = tSplit;
    buildTree(nodeNum + 1, bounds0, allPrimBounds, prims0, n0, depth - 1, edges,
        prims0, prims1 + nPrimitives, badRefines);
    int aboveChild = nextFreeNode;
    nodes[nodeNum].InitInterior(bestAxis, aboveChild, tSplit);
    buildTree(aboveChild, bounds1, allPrimBounds, prims1, n1, depth - 1, edges,
        prims0, prims1 + nPrimitives, badRefines);
}

bool KDTreeAccelerator::Intersect(const common::math::Rayf &ray, core::interaction::SurfaceInteraction *isect) const
{
    //ProfilePhase p(Prof::AccelIntersect);
    // Compute initial parametric range of ray inside kd-tree extent
    Float t_min, t_max;
    if (!bounds.IntersectP(ray, &t_min, &t_max))
    {
        return false;
    }

    // Prepare to traverse kd-tree for ray
    common::math::Vec3f invDir(FLOAT_1 / ray.dir.x, FLOAT_1 / ray.dir.y, FLOAT_1 / ray.dir.z);
    constexpr int maxTodo = 64;
    KDTreeToDo todo[maxTodo];
    int todoPos = 0;

    // Traverse kd-tree nodes in order for ray
    bool hit = false;
    const KDTreeNode *node = &nodes[0];
    while (node != nullptr)
    {
        // Bail out if we found a hit closer than the current node
        if (ray.t_max < t_min)
        {
            break;
        }
        if (!node->IsLeaf())
        {
            // Process kd-tree interior node

            // Compute parametric distance along ray to split plane
            int axis = node->SplitAxis();
            Float tPlane = (node->SplitPos() - ray.origin[axis]) * invDir[axis];

            // Get node children pointers for ray
            const KDTreeNode *firstChild, *secondChild;
            int belowFirst =
                (ray.origin[axis] < node->SplitPos()) ||
                (ray.origin[axis] == node->SplitPos() && ray.dir[axis] <= 0);
            if (belowFirst)
            {
                firstChild = node + 1;
                secondChild = &nodes[node->AboveChild()];
            }
            else
            {
                firstChild = &nodes[node->AboveChild()];
                secondChild = node + 1;
            }

            // Advance to next child node, possibly enqueue other child
            if (tPlane > t_max || tPlane <= 0)
            {
                node = firstChild;
            }
            else if (tPlane < t_min)
            {
                node = secondChild;
            }
            else
            {
                // Enqueue _secondChild_ in todo list
                todo[todoPos].node = secondChild;
                todo[todoPos].t_min = tPlane;
                todo[todoPos].t_max = t_max;
                ++todoPos;
                node = firstChild;
                t_max = tPlane;
            }
        }
        else
        {
            // Check for intersections inside leaf node
            int nPrimitives = node->nPrimitives();
            if (1 == nPrimitives)
            {
                const std::shared_ptr<core::primitive::Primitive> &p =
                    primitives[node->onePrimitive];
                // Check one primitive inside leaf node
                if (p->Intersect(ray, isect))
                {
                    hit = true;
                }
            }
            else
            {
                for (int i = 0; i < nPrimitives; ++i)
                {
                    int index = primitiveIndices[node->primitiveIndicesOffset + i];
                    const std::shared_ptr<core::primitive::Primitive> &p = primitives[index];
                    // Check one primitive inside leaf node
                    if (p->Intersect(ray, isect))
                    {
                        hit = true;
                    }
                }
            }

            // Grab next node to process from todo list
            if (todoPos > 0)
            {
                --todoPos;
                node = todo[todoPos].node;
                t_min = todo[todoPos].t_min;
                t_max = todo[todoPos].t_max;
            }
            else
            {
                break;
            }
        }
    }
    return hit;
}

bool KDTreeAccelerator::IntersectP(const common::math::Rayf &ray) const
{
    //ProfilePhase p(Prof::AccelIntersectP);
    // Compute initial parametric range of ray inside kd-tree extent
    Float t_min, t_max;
    if (!bounds.IntersectP(ray, &t_min, &t_max))
    {
        return false;
    }

    // Prepare to traverse kd-tree for ray
    common::math::Vec3f invDir(1 / ray.dir.x, 1 / ray.dir.y, 1 / ray.dir.z);
    constexpr int maxTodo = 64;
    KDTreeToDo todo[maxTodo];
    int todoPos = 0;
    const KDTreeNode *node = &nodes[0];
    while (node != nullptr)
    {
        if (node->IsLeaf())
        {
            // Check for shadow ray intersections inside leaf node
            int nPrimitives = node->nPrimitives();
            if (1 == nPrimitives)
            {
                const std::shared_ptr<core::primitive::Primitive> &p =
                    primitives[node->onePrimitive];
                if (p->IntersectP(ray))
                {
                    return true;
                }
            }
            else
            {
                for (int i = 0; i < nPrimitives; ++i)
                {
                    int primitiveIndex = primitiveIndices[node->primitiveIndicesOffset + i];
                    const std::shared_ptr<core::primitive::Primitive> &prim =
                        primitives[primitiveIndex];
                    if (prim->IntersectP(ray))
                    {
                        return true;
                    }
                }
            }

            // Grab next node to process from todo list
            if (todoPos > 0)
            {
                --todoPos;
                node = todo[todoPos].node;
                t_min = todo[todoPos].t_min;
                t_max = todo[todoPos].t_max;
            }
            else
            {
                break;
            }
        }
        else
        {
            // Process kd-tree interior node

            // Compute parametric distance along ray to split plane
            int axis = node->SplitAxis();
            Float tPlane = (node->SplitPos() - ray.origin[axis]) * invDir[axis];

            // Get node children pointers for ray
            const KDTreeNode *firstChild, *secondChild;
            int belowFirst =
                (ray.origin[axis] < node->SplitPos()) ||
                (ray.origin[axis] == node->SplitPos() && ray.dir[axis] <= 0);
            if (belowFirst)
            {
                firstChild = node + 1;
                secondChild = &nodes[node->AboveChild()];
            }
            else
            {
                firstChild = &nodes[node->AboveChild()];
                secondChild = node + 1;
            }

            // Advance to next child node, possibly enqueue other child
            if (tPlane > t_max || tPlane <= 0)
            {
                node = firstChild;
            }
            else if (tPlane < t_min)
            {
                node = secondChild;
            }
            else
            {
                // Enqueue _secondChild_ in todo list
                todo[todoPos].node = secondChild;
                todo[todoPos].t_min = tPlane;
                todo[todoPos].t_max = t_max;
                ++todoPos;
                node = firstChild;
                t_max = tPlane;
            }
        }
    }
    return false;
}


}
}
}