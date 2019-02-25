#pragma once

#include "Constants.h"

namespace common
{
namespace math
{


template<typename T>
struct Vec3
{
    T x, y, z;

    enum
    {
        SIZE = 3
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline
        Vec3()
    {}

    __forceinline explicit
        Vec3(const T& a) : x(a), y(a), z(a)
    {}

    __forceinline explicit
        Vec3(const T& x, const T& y, const T& z) : x(x), y(y), z(z)
    {}


    __forceinline
        Vec3(const Vec3& other)
    {
        Clone(other);
    }

    template<typename T1> __forceinline
        Vec3(const Vec3<T1>& a) : x(static_cast<T>(a.x)), y(static_cast<T>(a.y)), z(static_cast<T>(a.z))
    {}

    template<typename T1> __forceinline
        Vec3& operator =(const Vec3<T1>& other)
    {
        Clone(other);
        return *this;
    }


    template<typename T1> __forceinline
        void Clone(const Vec3<T1>& other)
    {
        x = static_cast<T>(other.x);
        y = static_cast<T>(other.y);
        z = static_cast<T>(other.z);
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
Vec3<T> operator +(const Vec3<T>& a)
{
    return Vec3<T>(a.x, a.y, a.z);
}

template<typename T> __forceinline
Vec3<T> operator -(const Vec3<T>& a)
{
    return Vec3<T>(-a.x, -a.y, -a.z);
}

template<typename T> __forceinline
Vec3<T> Abs(const Vec3<T>& a)
{
    return Vec3<T>(abs(a.x), abs(a.y), abs(a.z));
}

template<typename T> __forceinline
Vec3<T> Rcp(const Vec3<T>& a)
{
    return Vec3<T>(rcp(a.x), rcp(a.y), rcp(a.z));
}

template<typename T> __forceinline
Vec3<T> Rsqrt(const Vec3<T>& a)
{
    return Vec3<T>(rsqrt(a.x), rsqrt(a.y), rsqrt(a.z));
}

template<typename T> __forceinline
Vec3<T> Sqrt(const Vec3<T>& a)
{
    return Vec3<T>(sqrt(a.x), sqrt(a.y), sqrt(a.z));
}


////////////////////////////////////////////////////////////////////////////////
// Euclidian Space Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
T Dot(const Vec3<T>& a, const Vec3<T>& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<typename T> __forceinline
T AbsDot(const Vec3<T>& a, const Vec3<T>& b)
{
    return abs(Dot(a, b));
}

template<typename T> __forceinline
T LengthSquared(const Vec3<T>& a)
{
    return Dot(a, a);
}

template<typename T> __forceinline
T Length(const Vec3<T>& a)
{
    return sqrt(LengthSquared(a));
}

template <typename T> __forceinline
T DistanceSquared(const Vec3<T> &a, const Vec3<T> &b)
{
    return LengthSquared(a - b);
}

template<typename T> __forceinline
T Distance(const Vec3<T>& a, const Vec3<T>& b)
{
    return Length(a - b);
}

template<typename T> __forceinline
Vec3<T> Normalize(const Vec3<T>& a)
{
    return a / Length(a);
}

template<typename T> __forceinline
Vec3<T> Cross(const Vec3<T>& a, const Vec3<T>& b)
{
    return Vec3<T>((a.y * b.z) - (a.z * b.y)
        , (a.z * b.x) - (a.x * b.z)
        , (a.x * b.y) - (a.y * b.x));
}

template<typename T> __forceinline
Vec3<T> Faceforward(const Vec3<T> &v, const Vec3<T> &v2)
{
    return (Dot(v, v2) < static_cast<T>(0.0F)) ? -v : v;
}


////////////////////////////////////////////////////////////////////////////////
// Binary Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
Vec3<T> operator +(const Vec3<T>& a, const Vec3<T>& b)
{
    return Vec3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
}

template<typename T> __forceinline
Vec3<T> operator -(const Vec3<T>& a, const Vec3<T>& b)
{
    return Vec3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
}

template<typename T> __forceinline
Vec3<T> operator *(const Vec3<T>& a, const Vec3<T>& b)
{
    return Vec3<T>(a.x * b.x, a.y * b.y, a.z * b.z);
}

template<typename T> __forceinline
Vec3<T> operator *(const T& a, const Vec3<T>& b)
{
    return Vec3<T>(a   * b.x, a   * b.y, a   * b.z);
}

template<typename T> __forceinline
Vec3<T> operator *(const Vec3<T>& a, const T& b)
{
    return Vec3<T>(a.x * b, a.y * b, a.z * b);
}

template<typename T> __forceinline
Vec3<T> operator /(const Vec3<T>& a, const T& b)
{
    return Vec3<T>(a.x / b, a.y / b, a.z / b);
}

template<typename T> __forceinline
Vec3<T> operator /(const T& a, const Vec3<T>& b)
{
    return Vec3<T>(a / b.x, a / b.y, a / b.z);
}

template<typename T> __forceinline
Vec3<T> operator /(const Vec3<T>& a, const Vec3<T>& b)
{
    return Vec3<T>(a.x / b.x, a.y / b.y, a.z / b.z);
}


template<typename T> __forceinline
Vec3<T> Min(const Vec3<T>& a, const Vec3<T>& b)
{
    return Vec3<T>((std::min)(a.x, b.x), (std::min)(a.y, b.y), (std::min)(a.z, b.z));
}

template<typename T> __forceinline
Vec3<T> Max(const Vec3<T>& a, const Vec3<T>& b)
{
    return Vec3<T>((std::max)(a.x, b.x), (std::max)(a.y, b.y), (std::max)(a.z, b.z));
}


template<typename T> __forceinline
Vec3<T> operator >> (const Vec3<T>& a, const int b)
{
    return Vec3<T>(a.x >> b, a.y >> b, a.z >> b);
}

template<typename T> __forceinline
Vec3<T> operator <<(const Vec3<T>& a, const int b)
{
    return Vec3<T>(a.x << b, a.y << b, a.z << b);
}

////////////////////////////////////////////////////////////////////////////////
// Assignment Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
const Vec3<T>& operator +=(Vec3<T>& a, const T b)
{
    a.x += b;
    a.y += b;
    a.z += b;
    return a;
}

template<typename T> __forceinline
const Vec3<T>& operator +=(Vec3<T>& a, const Vec3<T>& b)
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

template<typename T> __forceinline
const Vec3<T>& operator -=(Vec3<T>& a, const Vec3<T>& b)
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

template<typename T> __forceinline
const Vec3<T>& operator *=(Vec3<T>& a, const T& b)
{
    a.x *= b;
    a.y *= b;
    a.z *= b;
    return a;
}

template<typename T> __forceinline
const Vec3<T>& operator /=(Vec3<T>& a, const T& b)
{
    a.x /= b;
    a.y /= b;
    a.z /= b;
    return a;
}

////////////////////////////////////////////////////////////////////////////////
// Comparison Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
bool operator ==(const Vec3<T>& a, const Vec3<T>& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

template<typename T> __forceinline
bool operator !=(const Vec3<T>& a, const Vec3<T>& b)
{
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

template<typename T> __forceinline
bool operator < (const Vec3<T>& a, const Vec3<T>& b)
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
    return false;
}

////////////////////////////////////////////////////////////////////////////////
// Select
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
int MaxDim(const Vec3<T>& a)
{
    if (a.x > a.y)
    {
        if (a.x > a.z)
        {
            return 0;
        }
        else
        {
            return 2;
        }
    }
    else
    {
        if (a.y > a.z)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
}

template<typename T> __forceinline
int MinDim(const Vec3<T>& a)
{
    if (a.x <= a.y)
    {
        if (a.x <= a.z)
        {
            return 0;
        }
        else
        {
            return 2;
        }
    }
    else
    {
        if (a.y <= a.z)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
}

template<typename T> __forceinline
int MaxComponent(const Vec3<T>& a)
{
    return a[MaxDim(a)];
}

template<typename T> __forceinline
int MinComponent(const Vec3<T>& a)
{
    return a[MinDim(a)];
}

template <typename T> __forceinline
Vec3<T> Permute(const Vec3<T> &a, const size_t &x, const size_t &y, const size_t &z)
{
    return Vec3<T>(a[x], a[y], a[z]);
}

template <typename T> __forceinline
void CoordinateSystem(const Vec3<T> &v1, Vec3<T> *v2, Vec3<T> *v3)
{
    if (std::abs(v1.x) > std::abs(v1.y))
    {
        *v2 = Vec3<T>(-v1.z, 0, v1.x) / std::sqrt(v1.x * v1.x + v1.z * v1.z);
    }
    else
    {
        *v2 = Vec3<T>(0, v1.z, -v1.y) / std::sqrt(v1.y * v1.y + v1.z * v1.z);
    }

    *v3 = Cross(v1, *v2);
}

template <typename T> __forceinline
Vec3<T> SphericalDirection(T sinTheta, T cosTheta, T phi)
{
    return Vec3<T>(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);
}

template <typename T> __forceinline
Vec3<T> SphericalDirection(Float sinTheta, Float cosTheta, Float phi,
    const Vec3<T> &x, const Vec3<T> &y, const Vec3<T> &z)
{
    return sinTheta * std::cos(phi) * x + sinTheta * std::sin(phi) * y + cosTheta * z;
}

template <typename T> __forceinline
Float SphericalTheta(const Vec3<T> &v)
{
    return std::acos(Clamp(v.z, static_cast<T>(-1), static_cast<T>(1)));
}

template <typename T> __forceinline
Float SphericalPhi(const Vec3<T> &v)
{
    T p = std::atan2(v.y, v.x);
    return (p < static_cast<T>(0)) ? (p + static_cast<T>(2) * PI) : p;
}

////////////////////////////////////////////////////////////////////////////////
// Output Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> inline
std::ostream& operator<<(std::ostream& cout, const Vec3<T>& a)
{
    return cout << "(" << a.x << ", " << a.y << ", " << a.z << ")";
}


}
}