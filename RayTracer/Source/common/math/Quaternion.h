#pragma once

#include "../../ForwardDeclaration.h"
#include "Constants.h"
#include "Vec3.h"
#include "Mat4.h"

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

    Quaternion(const Transform<T> &t)
    {
        const Mat4<T> &mat4 = t.mat4;
        T trace = mat4[0][0] + mat4[1][1] + mat4[2][2];
        if (trace > static_cast<T>(0.0F))
        {
            // Compute w from matrix trace, then xyz
            // 4w^2 = m[0][0] + m[1][1] + m[2][2] + m[3][3] (but m[3][3] == 1)
            T s = std::sqrt(trace + static_cast<T>(1.0F));
            w = s / static_cast<T>(2.0F);
            s = static_cast<T>(0.5F) / s;

            v.x = (mat4[2][1] - mat4[1][2]) * s;
            v.y = (mat4[0][2] - mat4[2][0]) * s;
            v.z = (mat4[1][0] - mat4[0][1]) * s;
        }
        else
        {
            // Compute largest of $x$, $y$, or $z$, then remaining components
            const int nxt[3] = {1, 2, 0};
            T q[3];
            int i = 0;
            if (mat4[1][1] > mat4[0][0])
            {
                i = 1;
            }
            if (mat4[2][2] > mat4[i][i])
            {
                i = 2;
            }
            int j = nxt[i];
            int k = nxt[j];

            T s = std::sqrt((mat4[i][i] - (mat4[j][j] + mat4[k][k])) + static_cast<T>(1.0F));
            q[i] = s * static_cast<T>(0.5F);
            if (static_cast<T>(0.0F) != s)
            {
                s = static_cast<T>(0.5F) / s;
            }

            w = (mat4[k][j] - mat4[j][k]) * s;

            q[j] = (mat4[j][i] + mat4[i][j]) * s;
            q[k] = (mat4[k][i] + mat4[i][k]) * s;

            v.x = q[0];
            v.y = q[1];
            v.z = q[2];
        }
    }
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
    return a * rcp(b);
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
Quaternion<T> Slerp(T t, const Quaternion<T> &q1, const Quaternion<T> &q2)
{
    T cos_theta = Dot(q1, q2);
    if (cos_theta > static_cast<T>(0.9995F))
    {
        return Normalize((1 - t) * q1 + t * q2);
    }
    else
    {
        T theta = std::acos(common::math::Clamp(cos_theta, static_cast<T>(-1), static_cast<T>(1)));
        T theta_p = theta * t;
        Quaternion<T> qperp = Normalize(q2 - q1 * cos_theta);

        return q1 * std::cos(theta_p) + qperp * std::sin(theta_p);
    }
}

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