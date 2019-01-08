#pragma once

#include "../../common/math/Vec3.h"

namespace core
{
    namespace interaction
    {
        /// TODO
        class MediumInterface
        {

        };

        struct Interaction
        {
            common::math::Vec3f p;
            Float time;
            common::math::Vec3f p_error;
            common::math::Vec3f wo;
            common::math::Vec3f n;
            const MediumInterface *medium_interface;


            ////////////////////////////////////////////////////////////////////////////////
            /// Construction
            ////////////////////////////////////////////////////////////////////////////////

            Interaction() : time(FLOAT_0)
            {}

            Interaction(const common::math::Vec3f &p, const common::math::Vec3f &n, const common::math::Vec3f &p_error, const common::math::Vec3f &wo, Float time
                , const MediumInterface *medium_interface)
                : p(p), time(time), p_error(p_error), wo(Normalize(wo)), n(n)
                , medium_interface(medium_interface)
            {}


            bool IsSurfaceInteraction() const
            {
                return n != common::math::Vec3f(FLOAT_0);
            }
        };
    }
}