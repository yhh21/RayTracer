#pragma once

#include "common/math/Constants.h"
#include "common/math/Vec3.h"

class Medium;

using namespace Common::Math;

namespace Core
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


    template<typename T = double>
    class RayDifferential : public Ray<T>
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