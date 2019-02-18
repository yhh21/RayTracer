#pragma once

#include "Constants.h"

namespace common
{
namespace math
{


template<typename T>
struct Vec2
{
    T x, y;

    enum
    {
        SIZE = 2
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline
        Vec2()
    {}

    __forceinline explicit
        Vec2(const T& a) : x(a), y(a)
    {}

    __forceinline explicit
        Vec2(const T& x, const T& y) : x(x), y(y)
    {}


    __forceinline
        Vec2(const Vec2& other)
    {
        Clone(other);
    }

    template<typename T1> __forceinline
        Vec2(const Vec2<T1>& a) : x(static_cast<T>(a.x)), y(static_cast<T>(a.y))
    {}

    template<typename T1> __forceinline
        Vec2& operator =(const Vec2<T1>& other)
    {
        Clone(other);
        return *this;
    }


    template<typename T1> __forceinline
        void Clone(const Vec2<T1>& other)
    {
        x = static_cast<T>(other.x);
        y = static_cast<T>(other.y);
    }


    __forceinline
        const T& operator [](const size_t axis) const
    {
        CHECK_LT(axis, SIZE);
        return (&x)[axis];
    }

    __forceinline
        T& operator [](const size_t axis)
    {
        CHECK_LT(axis, SIZE);
        return (&x)[axis];
    }
};


////////////////////////////////////////////////////////////////////////////////
// Unary Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
Vec2<T> operator +(const Vec2<T>& a)
{
    return Vec2<T>(a.x, a.y);
}

template<typename T> __forceinline
Vec2<T> operator -(const Vec2<T>& a)
{
    return Vec2<T>(-a.x, -a.y);
}

template<typename T> __forceinline
Vec2<T> Abs(const Vec2<T>& a)
{
    return Vec2<T>(std::abs(a.x), std::abs(a.y));
}

template<typename T> __forceinline
Vec2<T> Rcp(const Vec2<T>& a)
{
    return Vec2<T>(rcp(a.x), r(a.y));
}

template<typename T> __forceinline
Vec2<T> Rsqrt(const Vec2<T>& a)
{
    return Vec2<T>(rsqrt(a.x), rsqrt(a.y));
}

template<typename T> __forceinline
Vec2<T> Sqrt(const Vec2<T>& a)
{
    return Vec2<T>(std::sqrt(a.x), std::sqrt(a.y));
}

template<typename T> __forceinline
Vec2<T> Floor(const Vec2<T>& a)
{
    return Vec2<T>(std::floor(a.x), std::floor(a.y));
}

template<typename T> __forceinline
Vec2<T> Ceil(const Vec2<T>& a)
{
    return Vec2<T>(std::ceil(a.x), std::ceil(a.y));
}

////////////////////////////////////////////////////////////////////////////////
// Euclidian Space Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
T Dot(const Vec2<T>& a, const Vec2<T>& b)
{
    return a.x * b.x + a.y * b.y;
}

template<typename T> __forceinline
T AbsDot(const Vec2<T>& a, const Vec2<T>& b)
{
    return abs(Dot(a, b));
}

template<typename T> __forceinline
T Length(const Vec2<T>& a)
{
    return sqrt(Dot(a, a));
}

template<typename T> __forceinline
Vec2<T> Normalize(const Vec2<T>& a)
{
    return a / sqrt(Dot(a, a));
}

template<typename T> __forceinline
T Distance(const Vec2<T>& a, const Vec2<T>& b)
{
    return Length(a - b);
}

template<typename T> __forceinline
T Det(const Vec2<T>& a, const Vec2<T>& b)
{
    return a.x * b.y - a.y * b.x;
}

////////////////////////////////////////////////////////////////////////////////
// Binary Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
Vec2<T> operator +(const Vec2<T>& a, const Vec2<T>& b)
{
    return Vec2<T>(a.x + b.x, a.y + b.y);
}

template<typename T> __forceinline
Vec2<T> operator -(const Vec2<T>& a, const Vec2<T>& b)
{
    return Vec2<T>(a.x - b.x, a.y - b.y);
}

template<typename T> __forceinline
Vec2<T> operator *(const Vec2<T>& a, const Vec2<T>& b)
{
    return Vec2<T>(a.x * b.x, a.y * b.y);
}

template<typename T> __forceinline
Vec2<T> operator *(const T& a, const Vec2<T>& b)
{
    return Vec2<T>(a * b.x, a * b.y);
}

template<typename T> __forceinline
Vec2<T> operator *(const Vec2<T>& a, const T& b)
{
    return Vec2<T>(a.x * b, a.y * b);
}

template<typename T> __forceinline
Vec2<T> operator /(const Vec2<T>& a, const Vec2<T>& b)
{
    return Vec2<T>(a.x / b.x, a.y / b.y);
}

template<typename T> __forceinline
Vec2<T> operator /(const Vec2<T>& a, const T& b)
{
    return Vec2<T>(a.x / b, a.y / b);
}

template<typename T> __forceinline
Vec2<T> operator /(const T& a, const Vec2<T>& b)
{
    return Vec2<T>(a / b.x, a / b.y);
}


template<typename T> __forceinline
Vec2<T> Min(const Vec2<T>& a, const Vec2<T>& b)
{
    return Vec2<T>(min(a.x, b.x), min(a.y, b.y));
}

template<typename T> __forceinline
Vec2<T> Max(const Vec2<T>& a, const Vec2<T>& b)
{
    return Vec2<T>(max(a.x, b.x), max(a.y, b.y));
}

////////////////////////////////////////////////////////////////////////////////
// Assignment Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
Vec2<T>& operator +=(Vec2<T>& a, const Vec2<T>& b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}

template<typename T> __forceinline
Vec2<T>& operator -=(Vec2<T>& a, const Vec2<T>& b)
{
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

template<typename T> __forceinline
Vec2<T>& operator *=(Vec2<T>& a, const T& b)
{
    a.x *= b;
    a.y *= b;
    return a;
}

template<typename T> __forceinline
Vec2<T>& operator /=(Vec2<T>& a, const T& b)
{
    a.x /= b;
    a.y /= b;
    return a;
}

////////////////////////////////////////////////////////////////////////////////
// Comparison Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
bool operator ==(const Vec2<T>& a, const Vec2<T>& b)
{
    return a.x == b.x && a.y == b.y;
}

template<typename T> __forceinline
bool operator !=(const Vec2<T>& a, const Vec2<T>& b)
{
    return a.x != b.x || a.y != b.y;
}

template<typename T> __forceinline
bool operator < (const Vec2<T>& a, const Vec2<T>& b)
{
    if (a.x != b.x)
    {
        return a.x < b.x;
    }
    if (a.y != b.y)
    {
        return a.y < b.y;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Select
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
int MaxDim(const Vec2<T>& a)
{
    if (a.x > a.y)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

template<typename T> __forceinline
int MinDim(const Vec2<T>& a)
{
    if (a.x <= a.y)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

template<typename T> __forceinline
int MaxComponent(const Vec2<T>& a)
{
    return a[MaxDim(a)];
}

template<typename T> __forceinline
int MinComponent(const Vec2<T>& a)
{
    return a[MinDim(a)];
}

template <typename T> __forceinline
Vec2<T> Permute(const Vec2<T> &a, const size_t &x, const size_t &y)
{
    return Vec2<T>(a[x], a[y]);
}

////////////////////////////////////////////////////////////////////////////////
// Output Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> inline
std::ostream& operator<<(std::ostream& cout, const Vec2<T>& a)
{
    return cout << "(" << a.x << ", " << a.y << ")";
}


}
}