#pragma once

#include "Ray.h"

namespace common
{
    namespace math
    {
        template<typename T = double>
        class RayDifferential : public Ray<T>
        {
        public:
            bool has_differentials;
            Vec3T rx_origin, ry_origin;
            Vec3T rx_direction, ry_direction;


            ////////////////////////////////////////////////////////////////////////////////
            /// Construction
            ////////////////////////////////////////////////////////////////////////////////

            RayDifferential()
            {
                has_differentials = false;
            }

            RayDifferential(const Vec3T &orig, const Vec3T &dir
                , T t_max = std::numeric_limits<T>::infinity(), T t_min = static_cast<T>(0)
                , T time = static_cast<T>(0), const Medium *medium = nullptr)
                : Ray(orig, dir, t_max, t_min, time, medium)
            {
                has_differentials = false;
            }

            RayDifferential(const Ray &ray) : Ray(ray)
            {
                has_differentials = false;
            }


            void ScaleDifferentials(T s)
            {
                rx_origin = origin + (rx_origin - origin) * s;
                ry_origin = origin + (ry_origin - origin) * s;
                rx_direction = dir + (rx_direction - dir) * s;
                ry_direction = dir + (ry_direction - dir) * s;
            }
        };
    }
}