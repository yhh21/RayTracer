#pragma once

#include "../../ForwardDeclaration.h"
#include "Constants.h"
#include "Vec3.h"

namespace common
{
namespace math
{


template<typename T>
struct Quaternion
{
    Vec3<T> v;
    T w;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    __forceinline
        Quaternion() : v(Vec3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0))), w(static_cast<T>(1))
    {}

    __forceinline
        Quaternion(const Quaternion &other) : v(other.v), w(other.w)
    {}

    __forceinline
        Quaternion& operator=(const Quaternion &other)
    {
        v = other.v;
        w = other.w;
        return *this;
    }

    __forceinline
        Quaternion(const T &w) : v(Vec3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0))), w(w)
    {}

    __forceinline explicit
        Quaternion(const Vec3<T> &v) : w(static_cast<T>(0)), v(v)
    {}

    __forceinline
        Quaternion(const T &w, const T &x, const T &y, const T &z)
        : v(Vec3<T>(x, y, z)), w(w)
    {}

    __forceinline
        Quaternion(const T &w, const Vec3<T> &v) : w(w), v(v)
    {}

    Quaternion(const Transform<T> &t);
};


template<typename T> __forceinline
Quaternion<T> operator *(const T &a, const Quaternion<T> &b)
{
    return Quaternion<T>(a * b.w, a * b.v);
}

template<typename T> __forceinline
Quaternion<T> operator *(const Quaternion<T> &a, const T &b)
{
    return Quaternion<T>(a.w * b, a.v * b);
}

////////////////////////////////////////////////////////////////
// Unary Operators
////////////////////////////////////////////////////////////////

template<typename T> __forceinline
Quaternion<T> operator +(const Quaternion<T> &a)
{
    return Quaternion<T>(a);
}

template<typename T> __forceinline
Quaternion<T> operator -(const Quaternion<T> &a)
{
    return Quaternion<T>(-a.v, -a.w);
}

template<typename T> __forceinline
Quaternion<T> Conj(const Quaternion<T> &a)
{
    return Quaternion<T>(a.w, -a.v);
}

template<typename T> __forceinline
Quaternion<T> Rcp(const Quaternion<T> &a)
{
    return Conj(a) * rcp(AbsSquared(a));
}

template<typename T> __forceinline
T Abs(const Quaternion<T> &a)
{
    return sqrt(AbsSquared(a));
}

template<typename T> __forceinline
T AbsSquared(const Quaternion<T> &a)
{
    return a.w*a.w + LengthSquared(a.v);
}

template<typename T> __forceinline
Quaternion<T> Normalize(const Quaternion<T> &a)
{
    return a / Abs(a);
}

////////////////////////////////////////////////////////////////
// Binary Operators
////////////////////////////////////////////////////////////////

template<typename T> __forceinline
Quaternion<T> operator +(const T &a, const Quaternion<T> &b)
{
    return Quaternion<T>(a + b.w, b.v);
}

template<typename T> __forceinline
Quaternion<T> operator +(const Quaternion<T> &a, const T &b)
{
    return Quaternion<T>(a.w + b, a.v);
}

template<typename T> __forceinline
Quaternion<T> operator +(const Quaternion<T> &a, const Quaternion<T> &b)
{
    return Quaternion<T>(a.w + b.w, a.v + b.v);
}

template<typename T> __forceinline
Quaternion<T> operator -(const T &a, const Quaternion<T> &b)
{
    return Quaternion<T>(a - b.w, -b.v);
}

template<typename T> __forceinline
Quaternion<T> operator -(const Quaternion<T> &a, const T &b)
{
    return Quaternion<T>(a.w - b, a.v);
}

template<typename T> __forceinline
Quaternion<T> operator -(const Quaternion<T> &a, const Quaternion<T> &b)
{
    return Quaternion<T>(a.w - b.w, a.v - b.v);
}


template<typename T> __forceinline
Vec3<T> operator *(const Quaternion<T> &a, const Vec3<T> &b)
{
    return (a*Quaternion<T>(b)*Conj(a)).v;
}

template<typename T> __forceinline
Quaternion<T> operator *(const Quaternion<T> &a, const Quaternion<T> &b)
{
    return Quaternion<T>(a.w*b.w - Dot(a.v, b.v)
        , a.w*b.v[0] + a.v[0] * b.w + a.v[1] * b.v[2] - a.v[2] * b.v[1]
        , a.w*b.v[1] - a.v[0] * b.v[2] + a.v[1] * b.w + a.v[2] * b.v[0]
        , a.w*b.v[2] + a.v[0] * b.v[1] - a.v[1] * b.v[0] + a.v[2] * b.w);
}

template<typename T> __forceinline
Quaternion<T> operator /(const T &a, const Quaternion<T> &b)
{
    return a * Rcp(b);
}

template<typename T> __forceinline
Quaternion<T> operator /(const Quaternion<T> &a, const T &b)
{
    return a * Rcp(b);
}

template<typename T> __forceinline
Quaternion<T> operator /(const Quaternion<T> &a, const Quaternion<T> &b)
{
    return a * Rcp(b);
}

template<typename T> __forceinline
Quaternion<T>& operator +=(Quaternion<T> &a, const T &b)
{
    return a = a + b;
}

template<typename T> __forceinline
Quaternion<T>& operator +=(Quaternion<T> &a, const Quaternion<T> &b)
{
    return a = a + b;
}

template<typename T> __forceinline
Quaternion<T>& operator -=(Quaternion<T> &a, const T &b)
{
    return a = a - b;
}

template<typename T> __forceinline
Quaternion<T>& operator -=(Quaternion<T> &a, const Quaternion<T> &b)
{
    return a = a - b;
}

template<typename T> __forceinline
Quaternion<T>& operator *=(Quaternion<T> &a, const T &b)
{
    return a = a * b;
}

template<typename T> __forceinline
Quaternion<T>& operator *=(Quaternion<T> &a, const Quaternion<T> &b)
{
    return a = a * b;
}

template<typename T> __forceinline
Quaternion<T>& operator /=(Quaternion<T> &a, const T &b)
{
    return a = a * rcp(b);
}

template<typename T> __forceinline
Quaternion<T>& operator /=(Quaternion<T> &a, const Quaternion<T> &b)
{
    return a = a * rcp(b);
}

template<typename T> __forceinline
T Dot(const Quaternion<T> &q1, const Quaternion<T> &q2)
{
    return Dot(q1.v, q2.v) + q1.w * q2.w;
}

template<typename T> __forceinline
Quaternion<T> Slerp(T t, const Quaternion<T> &q1, const Quaternion<T> &q2);

////////////////////////////////////////////////////////////////////////////////
// Comparison Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
bool operator ==(const Quaternion<T> &a, const Quaternion<T> &b)
{
    return a.w == b.w && a.v == b.v;
}

template<typename T> __forceinline
bool operator !=(const Quaternion<T> &a, const Quaternion<T> &b)
{
    return !(a == b);
}


////////////////////////////////////////////////////////////////////////////////
// Output Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> static
std::ostream& operator<<(std::ostream &cout, const Quaternion<T> &q)
{
    return cout << "{ w = " << q.w << ", v = " << q.v << " }";
}


}
}