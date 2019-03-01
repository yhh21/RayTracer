#pragma once

#include "../../../ForwardDeclaration.h"

namespace common
{
namespace tool
{
namespace kdtree
{


enum class EdgeType
{
    Start,
    End
};

struct BoundEdge
{
    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    BoundEdge()
    {}

    BoundEdge(Float t, int primNum, bool starting) : t(t), primNum(primNum)
    {
        type = starting ? EdgeType::Start : EdgeType::End;
    }


    Float t;

    int primNum;

    EdgeType type;
};


}
}
}