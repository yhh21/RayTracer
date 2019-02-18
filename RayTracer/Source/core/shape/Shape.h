#pragma once

#include "../../ForwardDeclaration.h"
#include "../../common/math/Ray.h"
#include "../../common/math/Transform.h"

namespace core
{
namespace shape
{


class Shape
{
public:

    const common::math::Transformf *object_to_world, *world_to_object;
    const bool reverse_orientation;
    const bool transform_swaps_handedness;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    Shape::Shape(const common::math::Transformf *object_to_world, const common::math::Transformf *world_to_object
        , bool reverse_orientation);

    virtual ~Shape()
    {}


    virtual common::math::Bounds3f ObjectBound() const = 0;

    virtual common::math::Bounds3f WorldBound() const;


    virtual bool Intersect(const common::math::Rayf &ray, Float *tHit, core::interaction::SurfaceInteraction *isect
        , bool testAlphaTexture = true) const = 0;

    virtual bool IntersectP(const common::math::Rayf &ray, bool testAlphaTexture = true) const
    {
        return Intersect(ray, nullptr, nullptr, testAlphaTexture);
    }


    virtual Float Area() const = 0;
};


}
}