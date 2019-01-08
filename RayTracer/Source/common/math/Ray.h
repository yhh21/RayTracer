#pragma once

#include "Constants.h"
#include "Vec3.h"

class Medium;

namespace common
{
    namespace math
    {
        template<typename T = double>
        class Ray
        {
        public:
            typedef Vec3<T> Vec3T;

            Vec3T origin, dir, inv_dir;
            T time;
            mutable T t_min, t_max;

            /// TODO
            const Medium *medium;


            ////////////////////////////////////////////////////////////////////////////////
            /// Construction
            ////////////////////////////////////////////////////////////////////////////////

            Ray() : t_max(std::numeric_limits<T>::infinity()), t_min(static_cast<T>(0))
                , time(static_cast<T>(0)), medium(nullptr)
            {}

            Ray(const Vec3T& origin = Vec3T(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0))
                , const Vec3T& direction = Vec3T(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1))
                , T t_max = (std::numeric_limits<T>::max)(), T t_min = static_cast<T>(0)
                , T time = static_cast<T>(0))
                : origin(origin), dir(direction), inv_dir(-dir), t_max(t_max), t_min(t_min)
            {}


            __forceinline
                Vec3T operator()(T t) const
            {
                return origin + dir * t;
            }
        };

        ////////////////////////////////////////////////////////////////////////////////
        /// Default template instantiations
        ////////////////////////////////////////////////////////////////////////////////

        typedef Ray<Float> Rayf;
    }
}