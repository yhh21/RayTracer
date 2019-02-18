#include "GeometricPrimitive.h"
#include "../../common/math/Bounds3.h"
#include "../../common/math/Ray.h"
#include "../shape/Shape.h"
#include "../interaction/SurfaceInteraction.h"
#include "../primitive/Primitive.h"


namespace core
{
namespace primitive
{


GeometricPrimitive::GeometricPrimitive(const std::shared_ptr<shape::Shape> &shape,
    const std::shared_ptr<core::material::Material> &material,
    const std::shared_ptr<light::AreaLight> &areaLight,
    const interaction::MediumInterface &medium_interface)
    : shape(shape),
    material(material),
    areaLight(areaLight),
    medium_interface(medium_interface)
{
    //primitiveMemory += sizeof(*this);
}

common::math::Bounds3f GeometricPrimitive::WorldBound() const
{
    return shape->WorldBound();
}

bool GeometricPrimitive::IntersectP(const common::math::Rayf &r) const
{
    return shape->IntersectP(r);
}

bool GeometricPrimitive::Intersect(const common::math::Rayf &r,
    interaction::SurfaceInteraction *isect) const
{
    Float tHit;
    if (!shape->Intersect(r, &tHit, isect))
    {
        return false;
    }
    r.t_max = tHit;
    isect->primitive = this;
    CHECK_GE(Dot(isect->n, isect->shading.n), FLOAT_0);
    // Initialize _SurfaceInteraction::mediumInterface_ after _Shape_
    // intersection
    if (medium_interface.IsMediumTransition())
    {
        isect->medium_interface = medium_interface;
    }
    else
    {
        isect->medium_interface = interaction::MediumInterface(r.medium);
    }
    return true;
}

const light::AreaLight *GeometricPrimitive::GetAreaLight() const
{
    return areaLight.get();
}

const core::material::Material *GeometricPrimitive::GetMaterial() const
{
    return material.get();
}

void GeometricPrimitive::ComputeScatteringFunctions(
    interaction::SurfaceInteraction *isect, common::tool::MemoryArena &arena, material::TransportMode mode,
    bool allowMultipleLobes) const
{
    //ProfilePhase p(Prof::ComputeScatteringFuncs);
    if (material)
    {
        material->ComputeScatteringFunctions(isect, arena, mode,
            allowMultipleLobes);
    }
    CHECK_GE(Dot(isect->n, isect->shading.n), FLOAT_0);
}


}
}