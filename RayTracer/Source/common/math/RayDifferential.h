#pragma once

#include "Ray.h"

namespace common
{
namespace math
{

template<typename T>
class RayDifferential : public Ray<T>
{
public:

    bool has_differentials;
    Vec3<T> rx_origin, ry_origin;
    Vec3<T> rx_direction, ry_direction;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    RayDifferential()
    {
        has_differentials = false;
    }

    RayDifferential(const Vec3<T>& origin, const Vec3<T>& direction
        , T t_max = (std::numeric_limits<T>::max)(), T t_min = static_cast<T>(0)
        , T time = static_cast<T>(0), const core::interaction::Medium *medium = nullptr)
        : Ray(origin, direction, t_max, t_min, time, medium)
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