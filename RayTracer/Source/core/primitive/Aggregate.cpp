#include "Aggregate.h"


namespace core
{
namespace primitive
{


const light::AreaLight *Aggregate::GetAreaLight() const
{
    /* TODO
    LOG(FATAL) <<
        "Aggregate::GetAreaLight() method"
        "called; should have gone to GeometricPrimitive";
    */
    return nullptr;
}

const material::Material *Aggregate::GetMaterial() const
{
    /* TODO
    LOG(FATAL) <<
        "Aggregate::GetMaterial() method"
        "called; should have gone to GeometricPrimitive";
    */
    return nullptr;
}

void Aggregate::ComputeScatteringFunctions(interaction::SurfaceInteraction *isect,
    common::tool::MemoryArena &arena,
    material::TransportMode mode,
    bool allowMultipleLobes) const
{
    /* TODO
    LOG(FATAL) <<
        "Aggregate::ComputeScatteringFunctions() method"
        "called; should have gone to GeometricPrimitive";
    */
}


}
}