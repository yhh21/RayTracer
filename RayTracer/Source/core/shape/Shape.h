#pragma once

#include "../../ForwardDeclaration.h"

namespace core
{
namespace interaction
{
class SurfaceInteraction;
}

namespace shape
{
class Shape
{
public:

    const common::math::Transform<Float> *object_to_world, *world_to_object;
    const bool reverse_orientation;
    const bool transform_swaps_handedness;

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    Shape::Shape(const common::math::Transform<Float> *object_to_world, const common::math::Transform<Float> *world_to_object
        , bool reverse_orientation);

    virtual ~Shape()
    {}


    virtual common::math::Bounds3<Float> ObjectBound() const = 0;

    virtual common::math::Bounds3<Float> WorldBound() const;


    virtual bool Intersect(const common::math::Ray<Float> &ray, Float *tHit, core::interaction::SurfaceInteraction *isect
        , bool testAlphaTexture = true) const = 0;

    virtual bool IntersectP(const common::math::Ray<Float> &ray, bool testAlphaTexture = true) const
    {
        return Intersect(ray, nullptr, nullptr, testAlphaTexture);
    }


    virtual Float Area() const = 0;
};


}
}