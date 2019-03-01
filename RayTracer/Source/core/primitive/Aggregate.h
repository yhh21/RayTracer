#pragma once

#include "Primitive.h"

namespace core
{
namespace primitive
{


class Aggregate : public Primitive
{
public:

    const light::AreaLight *GetAreaLight() const;

    const material::Material *GetMaterial() const;

    void ComputeScatteringFunctions(interaction::SurfaceInteraction *isect,
        common::tool::MemoryArena &arena,
        material::TransportMode mode,
        bool allowMultipleLobes) const;
};


}
}