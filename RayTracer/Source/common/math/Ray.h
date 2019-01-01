#pragma once

#include <cmath>
#include "Vec3.h"

class Medium;

namespace Utilities
{
    namespace Math
    {
        template<typename RealT = double>
        class Ray
        {
        public:
            typedef Vec3<RealT> Vec3T;

            Vec3T origin, dir, inv_dir;
            RealT time;
            mutable RealT t_min, t_max;

            /// TODO
            const Medium *medium;


            Ray() : t_max(std::numeric_limits<RealT>::infinity()), t_min(static_cast<RealT>(0))
                , time(static_cast<RealT>(0)), medium(nullptr)
            {}

            Ray(const Vec3T& origin = Vec3T(static_cast<RealT>(0), static_cast<RealT>(0), static_cast<RealT>(0))
                , const Vec3T& direction = Vec3T(static_cast<RealT>(0), static_cast<RealT>(0), static_cast<RealT>(1))
                , RealT t_max = std::numeric_limits<RealT>::max(), RealT t_min = static_cast<RealT>(0)
                , RealT time = static_cast<RealT>(0))
                : origin(origin), dir(direction), inv_dir(1 / direction), t_max(t_max), t_min(t_min)
            {}


            Vec3T operator()(RealT t) const
            {
                return origin + dir * t;
            }
        }


        template<typename RealT = double>
        class RayDifferential : public Ray<RealT>
        {
        public:
            bool has_differentials;
            Vec3T rx_origin, ry_origin;
            Vec3T rx_direction, ry_direction;


            RayDifferential()
            {
                has_differentials = false;
            }

            RayDifferential(const Vec3T &orig, const Vec3T &dir
                , RealT t_max = std::numeric_limits<RealT>::infinity(), RealT t_min = static_cast<RealT>(0)
                , RealT time = static_cast<RealT>(0) , const Medium *medium = nullptr)
                : Ray(orig, dir, t_max, t_min, time, medium)
            {
                has_differentials = false;
            }
                
            RayDifferential(const Ray &ray) : Ray(ray)
            {
                has_differentials = false;
            }


            void ScaleDifferentials(RealT s)
            {
                rx_origin = origin + (rx_origin - origin) * s;
                ry_origin = origin + (ry_origin - origin) * s;
                rx_direction = dir + (rx_direction - dir) * s;
                ry_direction = dir + (ry_direction - dir) * s;
            }
        };
    }
}