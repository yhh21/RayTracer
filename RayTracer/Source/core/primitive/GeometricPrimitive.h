#pragma once

#include "Primitive.h"
#include "../interaction/MediumInteraction.h"

namespace core
{
namespace primitive
{


class GeometricPrimitive : public Primitive
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    GeometricPrimitive(const std::shared_ptr<core::shape::Shape> &shape,
        const std::shared_ptr<core::material::Material> &material,
        const std::shared_ptr<light::AreaLight> &areaLight,
        const interaction::MediumInterface &medium_interface);


    virtual common::math::Bounds3f WorldBound() const;

    virtual bool Intersect(const common::math::Rayf &r, interaction::SurfaceInteraction *isect) const;

    virtual bool IntersectP(const common::math::Rayf &r) const;

    const light::AreaLight *GetAreaLight() const;

    const core::material::Material *GetMaterial() const;

    void ComputeScatteringFunctions(interaction::SurfaceInteraction *isect,
        common::tool::MemoryArena &arena, material::TransportMode mode,
        bool allowMultipleLobes) const;

private:

    std::shared_ptr<core::shape::Shape> shape;

    std::shared_ptr<core::material::Material> material;

    std::shared_ptr<light::AreaLight> areaLight;

    interaction::MediumInterface medium_interface;
};

}
}