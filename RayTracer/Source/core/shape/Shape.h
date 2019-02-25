#pragma once

#include "../../ForwardDeclaration.h"
#include "../../common/math/Ray.h"

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


    virtual interaction::Interaction Sample(const common::math::Vec2f &u, Float *pdf) const = 0;

    virtual Float Pdf(const interaction::Interaction &) const
    {
        return FLOAT_1 / Area();
    }

    // Sample a point on the shape given a reference point |ref| and
    // return the PDF with respect to solid angle from |ref|.
    virtual interaction::Interaction Sample(const interaction::Interaction &ref, const common::math::Vec2f &u,
        Float *pdf) const;

    virtual Float Pdf(const interaction::Interaction &ref, const common::math::Vec3f &wi) const;

    // Returns the solid angle subtended by the shape w.r.t. the reference
    // point p, given in world space. Some shapes compute this value in
    // closed-form, while the default implementation uses Monte Carlo
    // integration; the nSamples parameter determines how many samples are
    // used in this case.
    virtual Float SolidAngle(const common::math::Vec3f &p, int nSamples = 512) const;

};


}
}