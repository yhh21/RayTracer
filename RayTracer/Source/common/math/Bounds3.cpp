#include "Bounds3.h"

#include "Ray.h"

namespace common
{
namespace math
{


template<typename T>
bool Bounds3<T>::IntersectP(const Ray<T> &ray, T *hitt0, T *hitt1)
{
    T t0 = ray.t_min, t1 = ray.t_max;
    for (int i = 0; i < 3; ++i)
    {
        // Update interval for _i_th bounding box slab
        T inv_ray_dir = static_cast<T>(1.0F) / ray.dir[i];
        T t_near = (point_min[i] - ray.origin[i]) * inv_ray_dir;
        T t_far = (point_max[i] - ray.origin[i]) * inv_ray_dir;

        // Update parametric interval from slab intersection $t$ values
        if (t_near > t_far)
        {
            std::swap(t_near, t_far);
        }

        // Update _tFar_ to ensure robust ray--bounds intersection
        t_far *= static_cast<T>(1.0F) + static_cast<T>(2.0F) * static_cast<T>(Gamma(3));
        t0 = t_near > t0 ? t_near : t0;
        t1 = t_far < t1 ? t_far : t1;

        if (t0 > t1) return false;
    }

    if (nullptr != hitt0)
    {
        *hitt0 = t0;
    }
    if (nullptr != hitt1)
    {
        *hitt1 = t1;
    }

    return true;
}

template<typename T>
bool Bounds3<T>::IntersectP(const Ray<T> &ray, const Vec3<T> &inv_dir, const int dir_is_neg[3]) const
{
    const Bounds3f &bounds = *this;

    // Check for ray intersection against $x$ and $y$ slabs
    T t_min = (bounds[dir_is_neg[0]].x - ray.origin.x) * inv_dir.x;
    T t_max = (bounds[1 - dir_is_neg[0]].x - ray.origin.x) * inv_dir.x;
    T ty_min = (bounds[dir_is_neg[1]].y - ray.origin.y) * inv_dir.y;
    T ty_max = (bounds[1 - dir_is_neg[1]].y - ray.origin.y) * inv_dir.y;

    // Update _t_max_ and _ty_max_ to ensure robust bounds intersection
    t_max *= static_cast<T>(1.0F) + static_cast<T>(2.0F) * static_cast<T>(Gamma(3));
    ty_max *= static_cast<T>(1.0F) + static_cast<T>(2.0F) * static_cast<T>(Gamma(3));
    if (t_min > ty_max || ty_min > t_max) return false;
    if (ty_min > t_min)
    {
        t_min = ty_min;
    }
    if (ty_max < t_max)
    {
        t_max = ty_max;
    }

    // Check for ray intersection against $z$ slab
    T tz_min = (bounds[dir_is_neg[2]].z - ray.origin.z) * inv_dir.z;
    T tz_max = (bounds[1 - dir_is_neg[2]].z - ray.origin.z) * inv_dir.z;

    // Update _tz_max_ to ensure robust bounds intersection
    tz_max *= static_cast<T>(1.0F) + static_cast<T>(2.0F) * static_cast<T>(Gamma(3));
    if (t_min > tz_max || tz_min > t_max) return false;
    if (tz_min > t_min)
    {
        t_min = tz_min;
    }
    if (tz_max < t_max)
    {
        t_max = tz_max;
    }

    return (t_min < ray.t_max) && (t_max > static_cast<T>(0.0F));
}


}
}