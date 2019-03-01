#pragma once

#include "../../../ForwardDeclaration.h"
#include <vector>

namespace common
{
namespace tool
{
namespace kdtree
{


struct KDTreeNode
{
    union
    {
        Float split;                 // Interior
        int onePrimitive;            // Leaf
        int primitiveIndicesOffset;  // Leaf
    };


    void InitLeaf(int *primNums, int np, std::vector<int> *primitiveIndices)
    {
        flags = 3;
        nPrims |= (np << 2);
        // Store primitive ids for leaf node
        if (0 == np)
        {
            onePrimitive = 0;
        }
        else if (1 == np)
        {
            onePrimitive = primNums[0];
        }
        else
        {
            primitiveIndicesOffset = primitiveIndices->size();
            for (int i = 0; i < np; ++i)
            {
                primitiveIndices->push_back(primNums[i]);
            }
        }
    }

    void InitInterior(int axis, int ac, Float s)
    {
        split = s;
        flags = axis;
        aboveChild |= (ac << 2);
    }


    Float SplitPos() const
    {
        return split;
    }

    int nPrimitives() const
    {
        return nPrims >> 2;
    }

    int SplitAxis() const
    {
        return flags & 3;
    }

    bool IsLeaf() const
    {
        return 3 == (flags & 3);
    }

    int AboveChild() const
    {
        return aboveChild >> 2;
    }

private:
    union
    {
        int flags;       // Both
        int nPrims;      // Leaf
        int aboveChild;  // Interior
    };
};


}
}
}