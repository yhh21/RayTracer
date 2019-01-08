#pragma once

#include "Interaction.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Vec3.h"
#include "../shape/Shape.h"

namespace core
{
    namespace interaction
    {
        class SurfaceInteraction : public Interaction
        {
        public:

            common::math::Vec2f uv;
            common::math::Vec3f dp_du, dp_dv;
            common::math::Vec3f dn_du, dn_dv;
            const core::shape::Shape *shape = nullptr;
            struct
            {
                common::math::Vec3f n;
                common::math::Vec3f dp_du, dp_dv;
                common::math::Vec3f dn_du, dn_dv;
            } shading;

            //const Primitive *primitive = nullptr;
            //BSDF *bsdf = nullptr;
            //BSSRDF *bssrdf = nullptr;
            mutable common::math::Vec3f dp_dx, dp_dy;
            mutable Float du_dx = 0, dv_dx = 0, du_dy = 0, dv_dy = 0;

            // Added after book publication. Shapes can optionally provide a face
            // index with an intersection point for use in Ptex texture lookups.
            // If Ptex isn't being used, then this value is ignored.
            int face_index = 0;

            ////////////////////////////////////////////////////////////////////////////////
            /// Construction
            ////////////////////////////////////////////////////////////////////////////////

            SurfaceInteraction()
            {}

            SurfaceInteraction(const common::math::Vec3f &p, const common::math::Vec3f &p_error, const common::math::Vec2f &uv
                , const common::math::Vec3f &wo, const common::math::Vec3f &dp_du, const common::math::Vec3f &dp_dv
                , const common::math::Vec3f &dn_du, const common::math::Vec3f &dn_dv, Float time, const core::shape::Shape *shape
                , int face_index = 0)
                : Interaction(p, common::math::Vec3f(Normalize(Cross(dp_du, dp_dv))), p_error, wo, time, nullptr)
                , uv(uv), dp_du(dp_du), dp_dv(dp_dv), dn_du(dn_du), dn_dv(dn_dv)
                , shape(shape), face_index(face_index)
            {
                /// Initialize shading geometry from true geometry
                shading.n = n;
                shading.dp_du = dp_du;
                shading.dp_dv = dp_dv;
                shading.dn_du = dn_du;
                shading.dn_dv = dn_dv;

                /// Adjust normal based on orientation and handedness
                if (nullptr != shape && (shape->reverse_orientation ^ shape->transform_swaps_handedness))
                {
                    n *= static_cast<Float>(-1.0F);
                    shading.n *= static_cast<Float>(-1.0F);
                }
            }

            void SetShadingGeometry(const common::math::Vec3f &dp_dus, const common::math::Vec3f &dp_dvs
                , const common::math::Vec3f &dn_dus , const common::math::Vec3f &dn_dvs
                , bool orientation_is_authoritative)
            {
                /// Compute _shading.n_ for _SurfaceInteraction_
                shading.n = Normalize(Cross(dp_dus, dp_dvs));
                if (shape && (shape->reverse_orientation ^ shape->transform_swaps_handedness))
                {
                    shading.n = -shading.n;
                }

                if (orientation_is_authoritative)
                {
                    n = Faceforward(n, shading.n);
                }
                else
                {
                    shading.n = Faceforward(shading.n, n);
                }

                /// Initialize _shading_ partial derivative values
                shading.dp_du = dp_dus;
                shading.dp_dv = dp_dvs;
                shading.dn_du = dn_dus;
                shading.dn_dv = dn_dvs;
            }
        };
    }
}