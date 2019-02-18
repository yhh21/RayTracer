#pragma once

#include "../../ForwardDeclaration.h"
#include "../material/Material.h"

namespace core
{
namespace primitive
{


class Primitive
{
public:
    virtual ~Primitive(){}

    virtual common::math::Bounds3f WorldBound() const = 0;

    virtual bool Intersect(const common::math::Rayf &r, interaction::SurfaceInteraction *) const = 0;

    virtual bool IntersectP(const common::math::Rayf &r) const = 0;

    virtual const light::AreaLight *GetAreaLight() const = 0;

    virtual const material::Material *GetMaterial() const = 0;

    virtual void ComputeScatteringFunctions(interaction::SurfaceInteraction *isect,
        common::tool::MemoryArena &arena,
        material::TransportMode mode,
        bool allowMultipleLobes) const = 0;
};


}
}