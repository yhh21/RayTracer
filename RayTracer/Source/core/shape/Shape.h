#pragma once

#include "common/math/Transform.h"

class SurfaceInteraction;

namespace Core
{
    namespace Shape
    {
        class Shape
        {
        public:

            const Transform *object_to_world, *world_to_object;
            const bool reverse_orientation;
            const bool transform_swaps_handedness;

            ////////////////////////////////////////////////////////////////////////////////
            /// Construction
            ////////////////////////////////////////////////////////////////////////////////

            Shape(const Transform *object_to_world, const Transform *world_to_object
                , bool reverse_orientation)
                : object_to_world(object_to_world), world_to_object(world_to_object)
                , reverse_orientation(reverse_orientation), transform_swaps_handedness(object_to_world->SwapsHandedness())
            {
                //++nShapesCreated;
            }

            virtual ~Shape()
            {}


            virtual Bounds3f ObjectBound() const = 0;

            virtual Bounds3f WorldBound() const
            {
                return (*object_to_world)(ObjectBound());
            }


            virtual bool Intersect(const Ray &ray, Float *tHit, SurfaceInteraction *isect
                , bool testAlphaTexture = true) const = 0;

            virtual bool IntersectP(const Ray &ray, bool testAlphaTexture = true) const
            {
                return Intersect(ray, nullptr, nullptr, testAlphaTexture);
            }


            virtual Float Area() const = 0;
        };
    }
}