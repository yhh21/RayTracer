#pragma once

#include "Constants.h"

namespace common
{
namespace math
{


template<typename T>
struct Vec4
{
    T x, y, z, w;

    enum
    {
        SIZE = 4
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline
        Vec4()
    {}

    __forceinline explicit
        Vec4(const T& a) : x(a), y(a), z(a), w(a)
    {}

    __forceinline explicit
        Vec4(const T& x, const T& y, const T& z, const T& w) : x(x), y(y), z(z), w(w)
    {}


    __forceinline
        Vec4(const Vec4& other)
    {
        Clone(other);
    }

    template<typename T1> __forceinline
        Vec4(const Vec4<T1>& a) : x(static_cast<T>(a.x)), y(static_cast<T>(a.y)), z(static_cast<T>(a.z)), w(static_cast<T>(a.w))
    {}

    template<typename T1> __forceinline
        Vec4& operator =(const Vec4<T1>& other)
    {
        Clone(other);
        return *this;
    }


    template<typename T1> __forceinline
        void Clone(const Vec4<T1>& other)
    {
        x = static_cast<T>(other.x);
        y = static_cast<T>(other.y);
        z = static_cast<T>(other.z);
        w = static_cast<T>(other.w);
    }


    __forceinline
        const T& operator [](const size_t axis) const
    {
    #ifdef DEBUG
        CHECK_LT(axis, SIZE);
    #endif // DEBUG
        return (&x)[axis];
    }

    __forceinline
        T& operator [](const size_t axis)
    {
    #ifdef DEBUG
        CHECK_LT(axis, SIZE);
    #endif // DEBUG
        return (&x)[axis];
    }
};


////////////////////////////////////////////////////////////////////////////////
// Unary Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
Vec4<T> operator +(const Vec4<T>& a)
{
    return Vec4<T>(a.x, a.y, a.z, a.w);
}

template<typename T> __forceinline
Vec4<T> operator -(const Vec4<T>& a)
{
    return Vec4<T>(-a.x, -a.y, -a.z, -a.w);
}

template<typename T> __forceinline
Vec4<T> Abs(const Vec4<T>& a)
{
    return Vec4<T>(abs(a.x), abs(a.y), abs(a.z), abs(a.w));
}

template<typename T> __forceinline
Vec4<T> Rcp(const Vec4<T>& a)
{
    return Vec4<T>(rcp(a.x), rcp(a.y), rcp(a.z), rcp(a.w));
}

template<typename T> __forceinline
Vec4<T> Rsqrt(const Vec4<T>& a)
{
    return Vec4<T>(rsqrt(a.x), rsqrt(a.y), rsqrt(a.z), rsqrt(a.w));
}

template<typename T> __forceinline
Vec4<T> Sqrt(const Vec4<T>& a)
{
    return Vec4<T>(sqrt(a.x), sqrt(a.y), sqrt(a.z), sqrt(a.w));
}


////////////////////////////////////////////////////////////////////////////////
// Euclidian Space Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
T Dot(const Vec4<T>& a, const Vec4<T>& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

template<typename T> __forceinline
T AbsDot(const Vec4<T>& a, const Vec4<T>& b)
{
    return abs(Dot(a, b));
}

template<typename T> __forceinline
T LengthSquared(const Vec4<T>& a)
{
    return Dot(a, a);
}

template<typename T> __forceinline
T Length(const Vec4<T>& a)
{
    return sqrt(LengthSquared(a));
}

template<typename T> __forceinline
T Distance(const Vec4<T>& a, const Vec4<T>& b)
{
    return Length(a - b);
}

template <typename T> __forceinline
T DistanceSquared(const Vec4<T> &a, const Vec4<T> &b)
{
    return LengthSquared(a - b);
}

template<typename T> __forceinline
Vec4<T> Normalize(const Vec4<T>& a)
{
    return a / sqrt(Dot(a, a));
}

////////////////////////////////////////////////////////////////////////////////
// Binary Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
Vec4<T> operator +(const Vec4<T>& a, const Vec4<T>& b)
{
    return Vec4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

template<typename T> __forceinline
Vec4<T> operator -(const Vec4<T>& a, const Vec4<T>& b)
{
    return Vec4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

template<typename T> __forceinline
Vec4<T> operator *(const Vec4<T>& a, const Vec4<T>& b)
{
    return Vec4<T>(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

template<typename T> __forceinline
Vec4<T> operator *(const T& a, const Vec4<T>& b)
{
    return Vec4<T>(a   * b.x, a   * b.y, a   * b.z, a   * b.w);
}

template<typename T> __forceinline
Vec4<T> operator *(const Vec4<T>& a, const T& b)
{
    return Vec4<T>(a.x * b, a.y * b, a.z * b, a.w * b);
}

template<typename T> __forceinline
Vec4<T> operator /(const Vec4<T>& a, const Vec4<T>& b)
{
    return Vec4<T>(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

template<typename T> __forceinline
Vec4<T> operator /(const Vec4<T>& a, const T& b)
{
    return Vec4<T>(a.x / b, a.y / b, a.z / b, a.w / b);
}

template<typename T> __forceinline
Vec4<T> operator /(const T& a, const Vec4<T>& b)
{
    return Vec4<T>(a / b.x, a / b.y, a / b.z, a / b.w);
}


template<typename T> __forceinline
Vec4<T> Min(const Vec4<T>& a, const Vec4<T>& b)
{
    return Vec4<T>((std::min)(a.x, b.x), (std::min)(a.y, b.y), (std::min)(a.z, b.z), (std::min)(a.w, b.w));
}

template<typename T> __forceinline
Vec4<T> Max(const Vec4<T>& a, const Vec4<T>& b)
{
    return Vec4<T>((std::max)(a.x, b.x), (std::max)(a.y, b.y), (std::max)(a.z, b.z), (std::max)(a.w, b.w));
}

////////////////////////////////////////////////////////////////////////////////
// Assignment Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
Vec4<T>& operator +=(Vec4<T>& a, const Vec4<T>& b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    a.w += b.w;
    return a;
}

template<typename T> __forceinline
Vec4<T>& operator -=(Vec4<T>& a, const Vec4<T>& b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    a.w -= b.w;
    return a;
}

template<typename T> __forceinline
Vec4<T>& operator *=(Vec4<T>& a, const T& b)
{
    a.x *= b;
    a.y *= b;
    a.z *= b;
    a.w *= b;
    return a;
}

template<typename T> __forceinline
Vec4<T>& operator /=(Vec4<T>& a, const T& b)
{
    a.x /= b;
    a.y /= b;
    a.z /= b;
    a.w /= b;
    return a;
}

////////////////////////////////////////////////////////////////////////////////
// Comparison Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
bool operator ==(const Vec4<T>& a, const Vec4<T>& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

template<typename T> __forceinline
bool operator !=(const Vec4<T>& a, const Vec4<T>& b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}

template<typename T> __forceinline
bool operator < (const Vec4<T>& a, const Vec4<T>& b)
{
    if (a.x != b.x)
    {
        return a.x < b.x;
    }
    if (a.y != b.y)
    {
        return a.y < b.y;
    }
    if (a.z != b.z)
    {
        return a.z < b.z;
    }
    if (a.w != b.w)
    {
        return a.w < b.w;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Select
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
int MaxDim(const Vec4<T>& a)
{
    int ret = 0;
    for (int i = 1; i < a.SIZE; ++i)
    {
        if (a[ret] < a[i])
        {
            ret = i;
        }
    }

    return ret;
}

template<typename T> __forceinline
int MinDim(const Vec4<T>& a)
{
    int ret = 0;
    for (int i = 1; i < a.SIZE; ++i)
    {
        if (a[ret] > a[i])
        {
            ret = i;
        }
    }

    return ret;
}

template<typename T> __forceinline
int MaxComponent(const Vec4<T>& a)
{
    return a[MaxDim(a)];
}

template<typename T> __forceinline
int MinComponent(const Vec4<T>& a)
{
    return a[MinDim(a)];
}

template <typename T> __forceinline
Vec4<T> Permute(const Vec4<T> &a, const size_t &x, const size_t &y, const size_t &z, const size_t &w)
{
    return Vec4<T>(a[x], a[y], a[z], a[w]);
}

////////////////////////////////////////////////////////////////////////////////
// Output Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> inline
std::ostream& operator<<(std::ostream& cout, const Vec4<T>& a)
{
    return cout << "(" << a.x << ", " << a.y << ", " << a.z << ", " << a.w << ")";
}


}
}