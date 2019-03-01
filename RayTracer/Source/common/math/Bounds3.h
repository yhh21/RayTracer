#pragma once

#include "../../ForwardDeclaration.h"
#include "Constants.h"
#include "Vec3.h"
#include "Ray.h"

namespace common
{
namespace math
{


template<typename T>
class Bounds3
{
public:
    Vec3<T> point_min, point_max;

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    Bounds3()
    {
        /*
        T min_num = std::numeric_limits<T>::lowest();
        T max_num = (std::numeric_limits<T>::max)();
        point_min = Vec3<T>(min_num);
        point_max = Vec3<T>(max_num);
        */
    }

    explicit
        Bounds3(const Vec3<T> &p) : point_min(p), point_max(p)
    {}

    Bounds3(const Vec3<T> &p1, const Vec3<T> &p2)
        : point_min(Min(p1, p2)), point_max(Max(p1, p2))
    {}


    template <typename U> explicit
        operator Bounds3<U>() const
    {
        return Bounds3<U>((Vec3<U>)point_min, (Vec3<U>)point_max);
    }


    __forceinline
        const Vec3<T> &operator[](size_t axis) const
    {
        CHECK_LT(axis, 2);
        return (0 == axis) ? point_min : point_max;
    }

    __forceinline
        Vec3<T> &operator[](size_t axis)
    {
        CHECK_LT(axis, 2);
        return (1 == axis) ? point_min : point_max;
    }


    Vec3<T> Corner(int corner) const
    {
        CHECK_GE(corner, 0);
        CHECK_LT(corner, 8);
        return Vec3<T>((*this)[(corner & 1)].x,
            (*this)[(corner & 2) ? 1 : 0].y,
            (*this)[(corner & 4) ? 1 : 0].z);
    }

    Vec3<T> Diagonal() const
    {
        return point_max - point_min;
    }

    T SurfaceArea() const
    {
        Vec3<T> d = Diagonal();
        return static_cast<T>(2) * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    T Volume() const
    {
        Vec3<T> d = Diagonal();
        return d.x * d.y * d.z;
    }


    int MaximumExtent() const
    {
        Vec3<T> d = Diagonal();
        if (d.x > d.y && d.x > d.z) return 0;
        else if (d.y > d.z) return 1;
        else return 2;
    }

    Vec3<T> Lerp(const Vec3<T> &t) const
    {
        return Vec3<T>(common::math::Lerp(t.x, point_min.x, point_max.x)
            , common::math::Lerp(t.y, point_min.y, point_max.y)
            , common::math::Lerp(t.z, point_min.z, point_max.z));
    }

    Vec3<T> Offset(const Vec3<T> &p) const
    {
        Vec3<T> o = p - point_min;
        if (point_max.x > point_min.x)
        {
            o.x /= point_max.x - point_min.x;
        }
        if (point_max.y > point_min.y)
        {
            o.y /= point_max.y - point_min.y;
        }
        if (point_max.z > point_min.z)
        {
            o.z /= point_max.z - point_min.z;
        }
        return o;
    }

    void BoundingSphere(Vec3<T> *center, T *radius) const
    {
        *center = (point_min + point_max) * static_cast<T>(0.5);
        *radius = Inside(*center, *this) ? Distance(*center, point_max) : static_cast<T>(0);
    }

    void GetBoundingSphere(Vec3<T> *center, T *radius) const
    {
        *center = (point_min + point_max) * static_cast<T>(0.5);
        *radius = Inside(*center, *this) ? Distance(*center, point_max) : 0;
    }


    bool IntersectP(const Ray<T> &ray, T *hitt0 = nullptr, T *hitt1 = nullptr) const
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

    bool IntersectP(const Ray<T> &ray, const Vec3<T> &inv_dir, const int dir_is_neg[3]) const
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


};


template <typename T>
Bounds3<T> Union(const Bounds3<T> &b, const Vec3<T> &p)
{
    Bounds3<T> ret;
    ret.point_min = Min(b.point_min, p);
    ret.point_max = Max(b.point_max, p);
    return ret;
}

template <typename T>
Bounds3<T> Union(const Bounds3<T> &b1, const Bounds3<T> &b2)
{
    Bounds3<T> ret;
    ret.point_min = Min(b1.point_min, b2.point_min);
    ret.point_max = Max(b1.point_max, b2.point_max);
    return ret;
}


template <typename T>
Bounds3<T> Intersect(const Bounds3<T> &b1, const Bounds3<T> &b2)
{
    // Important: assign to point_min/point_max directly and don't run the Bounds3()
    // constructor, since it takes min/max of the points passed to it.  In
    // turn, that breaks returning an invalid bound for the case where we
    // intersect non-overlapping bounds (as we'd like to happen).
    Bounds3<T> ret;
    ret.point_min = Max(b1.point_min, b2.point_min);
    ret.point_max = Min(b1.point_max, b2.point_max);
    return ret;
}

template <typename T>
bool Overlaps(const Bounds3<T> &b1, const Bounds3<T> &b2)
{
    return (b1.point_max.x >= b2.point_min.x) && (b1.point_min.x <= b2.point_max.x)
        && (b1.point_max.y >= b2.point_min.y) && (b1.point_min.y <= b2.point_max.y)
        && (b1.point_max.z >= b2.point_min.z) && (b1.point_min.z <= b2.point_max.z);
}

template <typename T>
bool Inside(const Vec3<T> &p, const Bounds3<T> &b)
{
    return (p.x >= b.point_min.x && p.x <= b.point_max.x)
        && (p.y >= b.point_min.y && p.y <= b.point_max.y)
        && (p.z >= b.point_min.z && p.z <= b.point_max.z);
}

template <typename T>
bool InsideExclusive(const Vec3<T> &p, const Bounds3<T> &b)
{
    return (p.x >= b.point_min.x && p.x < b.point_max.x)
        && (p.y >= b.point_min.y && p.y < b.point_max.y)
        && (p.z >= b.point_min.z && p.z < b.point_max.z);
}


template <typename T, typename U> inline
Bounds3<T> Expand(const Bounds3<T> &b, U delta)
{
    return Bounds3<T>(b.point_min - Vec3<T>(delta, delta, delta)
        , b.point_max + Vec3<T>(delta, delta, delta));
}

////////////////////////////////////////////////////////////////////////////////
// Output Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> inline
std::ostream& operator<<(std::ostream& cout, const Bounds3<T>& a)
{
    return cout << "[ " << b.point_min << " - " << b.point_max << " ]";
}


}
}