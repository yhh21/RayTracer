#pragma once

#include "Constants.h"
#include "Vec2.h"
#include "Vec3.h"
#include <iterator>

namespace common
{
namespace math
{


template<typename T>
class Bounds2
{
public:
    Vec2<T> point_min, point_max;

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    Bounds2()
    {
        /*
        T min_num = std::numeric_limits<T>::lowest();
        T max_num = (std::numeric_limits<T>::max)();
        point_min = Vec2<T>(min_num);
        point_max = Vec2<T>(max_num);
        */
    }

    explicit Bounds2(const Vec2<T> &p) : point_min(p), point_max(p)
    {}

    Bounds2(const Vec2<T> &p1, const Vec2<T> &p2)
        : point_min(Min(p1, p2)), point_max(Max(p1, p2))
    {}


    template <typename U> explicit
        operator Bounds2<U>() const
    {
        return Bounds2<U>((Vec2<U>)point_min, (Vec2<U>)point_max);
    }


    __forceinline
        const Vec2<T> &operator[](size_t axis) const
    {
        CHECK_LT(axis, 2);
        return (i == 0) ? point_min : point_max;
    }

    __forceinline
        Vec2<T> &operator[](size_t axis)
    {
        CHECK_LT(axis, 2);
        return (i == 1) ? point_min : point_max;
    }


    Vec2<T> Corner(size_t corner) const
    {
        CHECK_GE(corner, 0);
        CHECK_LT(corner, 2);
        return (*this)[corner];
    }

    Vec2<T> Diagonal() const
    {
        return point_max - point_min;
    }

    T Area() const
    {
        Vec2<T> d = Diagonal();
        return d.x * d.y;
    }


    int MaximumExtent() const
    {
        Vec2<T> d = Diagonal();
        return d.x > d.y ? 0 : 1;
    }

    Vec2<T> Lerp(const Vec2<T> &t) const
    {
        return Vec2<T>(common::math::Lerp(t.x, point_min.x, point_max.x)
            , common::math::Lerp(t.y, point_min.y, point_max.y));
    }

    Vec2<T> Offset(const Vec2<T> &p) const
    {
        Vec2<T> o = p - point_min;
        if (point_max.x > point_min.x)
        {
            o.x /= point_max.x - point_min.x;
        }
        if (point_max.y > point_min.y)
        {
            o.y /= point_max.y - point_min.y;
        }

        return o;
    }

    void BoundingSphere(Vec2<T> *center, T *radius) const
    {
        *center = (point_min + point_max) * static_cast<T>(0.5);
        *radius = Inside(*center, *this) ? Distance(*center, point_max) : static_cast<T>(0);
    }

    void GetBoundingSphere(Vec2<T> *center, T *radius) const
    {
        *center = (point_min + point_max) * static_cast<T>(0.5);
        *radius = Inside(*center, *this) ? Distance(*center, point_max) : static_cast<T>(0);
    }
};


template <typename T>
Bounds2<T> Union(const Bounds2<T> &b, const Vec2<T> &p)
{
    Bounds2<T> ret;
    ret.point_min = Min(b.point_min, p);
    ret.point_max = Max(b.point_max, p);
    return ret;
}

template <typename T>
Bounds2<T> Union(const Bounds2<T> &b, const Bounds2<T> &b2)
{
    Bounds2<T> ret;
    ret.point_min = Min(b.point_min, b2.point_min);
    ret.point_max = Max(b.point_max, b2.point_max);
    return ret;
}

template <typename T>
Bounds2<T> Intersect(const Bounds2<T> &b1, const Bounds2<T> &b2)
{
    // Important: assign to point_min/point_max directly and don't run the Bounds2()
    // constructor, since it takes min/max of the points passed to it.  In
    // turn, that breaks returning an invalid bound for the case where we
    // intersect non-overlapping bounds (as we'd like to happen).
    Bounds2<T> ret;
    ret.point_min = Max(b1.point_min, b2.point_min);
    ret.point_max = Min(b1.point_max, b2.point_max);
    return ret;
}

template <typename T>
bool Overlaps(const Bounds2<T> &ba, const Bounds2<T> &bb)
{
    return (ba.point_max.x >= bb.point_min.x) && (ba.point_min.x <= bb.point_max.x)
        && (ba.point_max.y >= bb.point_min.y) && (ba.point_min.y <= bb.point_max.y);
}

template <typename T>
bool Inside(const Vec2<T> &pt, const Bounds2<T> &b)
{
    return (pt.x >= b.point_min.x && pt.x <= b.point_max.x
        && pt.y >= b.point_min.y && pt.y <= b.point_max.y);
}

template <typename T>
bool InsideExclusive(const Vec2<T> &pt, const Bounds2<T> &b)
{
    return (pt.x >= b.point_min.x && pt.x < b.point_max.x
        && pt.y >= b.point_min.y && pt.y < b.point_max.y);
}

template <typename T, typename U>
Bounds2<T> Expand(const Bounds2<T> &b, U delta)
{
    return Bounds2<T>(b.point_min - Vec2<T>(static_cast<T>(delta), static_cast<T>(delta))
        , b.point_max + Vec2<T>(static_cast<T>(delta), static_cast<T>(delta)));
}

////////////////////////////////////////////////////////////////////////////////
// Iteractor Operators
////////////////////////////////////////////////////////////////////////////////

class Bounds2iIterator : public std::forward_iterator_tag
{
public:

    Bounds2iIterator(const Bounds2i &b, const Vec2i &pt)
        : p(pt), bounds(&b)
    {}


    __forceinline
    Bounds2iIterator operator++()
    {
        Advance();
        return *this;
    }

    __forceinline
    Bounds2iIterator operator++(int)
    {
        Bounds2iIterator old = *this;
        Advance();
        return old;
    }

    __forceinline
    bool operator==(const Bounds2iIterator &bi) const
    {
        return p == bi.p && bounds == bi.bounds;
    }

    __forceinline
    bool operator!=(const Bounds2iIterator &bi) const
    {
        return p != bi.p || bounds != bi.bounds;
    }

    __forceinline
    Vec2i operator*() const
    {
        return p;
    }

private:
    void Advance()
    {
        ++p.x;
        if (p.x == bounds->point_max.x)
        {
            p.x = bounds->point_min.x;
            ++p.y;
        }
    }

    Vec2i p;
    const Bounds2i *bounds;
};


__forceinline
Bounds2iIterator begin(const Bounds2i &b)
{
    return Bounds2iIterator(b, b.point_min);
}

__forceinline
Bounds2iIterator end(const Bounds2i &b)
{
    // Normally, the ending point is at the minimum x value and one past
    // the last valid y value.
    Vec2i pEnd(b.point_min.x, b.point_max.y);
    // However, if the bounds are degenerate, override the end point to
    // equal the start point so that any attempt to iterate over the bounds
    // exits out immediately.
    if (b.point_min.x >= b.point_max.x || b.point_min.y >= b.point_max.y)
    {
        pEnd = b.point_min;
    }
    return Bounds2iIterator(b, pEnd);
}

////////////////////////////////////////////////////////////////////////////////
// Output Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> inline
std::ostream& operator<<(std::ostream& cout, const Bounds2<T>& a)
{
    return cout << "[ " << b.point_min << " - " << b.point_max << " ]";
}


}
}