#pragma once

#include "Constants.h"
#include "Vec4.h"

namespace common
{
namespace math
{


template<typename T>
struct Mat4
{
    enum
    {
        SIZE = 4
    };

    T mat[SIZE][SIZE];


    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    Mat4(T a, T b, T c, T d
        , T e, T f, T g, T h
        , T i, T j, T k, T l
        , T m, T n, T o, T p)
    {
        mat[0][0] = static_cast<T>(a);
        mat[0][1] = static_cast<T>(b);
        mat[0][2] = static_cast<T>(c);
        mat[0][3] = static_cast<T>(d);

        mat[1][0] = static_cast<T>(e);
        mat[1][1] = static_cast<T>(f);
        mat[1][2] = static_cast<T>(g);
        mat[1][3] = static_cast<T>(h);

        mat[2][0] = static_cast<T>(i);
        mat[2][1] = static_cast<T>(j);
        mat[2][2] = static_cast<T>(k);
        mat[2][3] = static_cast<T>(l);

        mat[3][0] = static_cast<T>(m);
        mat[3][1] = static_cast<T>(n);
        mat[3][2] = static_cast<T>(o);
        mat[3][3] = static_cast<T>(p);
    }

    Mat4(const Vec4<T> &v1, const Vec4<T> &v2, const Vec4<T> &v3, const Vec4<T> &v4)
    {
        SetBasis(v1, v2, v3, v4);
    }

    Mat4(T **a)
    {
        CHECK_LT(sizeof(a) / sizeof(*a), SIZE);
        CHECK_LT(sizeof(*a) / sizeof(**a), SIZE);

        for (int i = 0; i < SIZE; ++i)
        {
            for (int j = 0; j < SIZE; ++j)
            {
                mat[i][j] = static_cast<T>(a[i][j]);
            }
        }
    }

    Mat4(const Mat4<T> &other)
    {
        Clone(other);
    }


    template<typename T1> __forceinline
        Mat4& operator =(const Mat4<T1>& other)
    {
        Clone(other);
        return *this;
    }


    template<typename T1> __forceinline
        void Clone(const Mat4<T1>& other)
    {
        for (int i = 0; i < SIZE; ++i)
        {
            for (int j = 0; j < SIZE; ++j)
            {
                mat[i][j] = static_cast<T>(other[i][j]);
            }
        }
    }

    void SetBasis(const Vec4<T> &v1, const Vec4<T> &v2, const Vec4<T> &v3)
    {
        for (int i = 0; i < SIZE; ++i)
        {
            mat[0][i] = v1[i];
            mat[1][i] = v2[i];
            mat[2][i] = v3[i];
            mat[3][i] = v4[i];
        }
    }


    __forceinline
        const T* operator[](size_t i) const
    {
        CHECK_LT(i, SIZE);
        return mat[i];
    }

    __forceinline
        T* operator[](size_t i)
    {
        CHECK_LT(i, SIZE);
        return mat[i];
    }

    __forceinline
        const T& operator()(size_t i, size_t j) const
    {
        CHECK_LT(i, SIZE);
        CHECK_LT(j, SIZE);
        return mat[i][j];
    }

    __forceinline
        T& operator()(size_t i, size_t j)
    {
        CHECK_LT(i, SIZE);
        CHECK_LT(j, SIZE);
        return mat[i][j];
    }
};


////////////////////////////////////////////////////////////////////////////////
// Unary Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
Mat4<T> operator+(const Mat4<T>& mat4)
{
    return Mat4<T>(mat4);
}

template<typename T> __forceinline
Mat4<T> operator-(const Mat4<T>& mat4)
{
    return Mat4<T>(-mat4[0][0], -mat4[0][1], -mat4[0][2], -mat4[0][3]
        , -mat4[1][0], -mat4[1][1], -mat4[1][2], -mat4[1][3]
        , -mat4[2][0], -mat4[2][1], -mat4[2][2], -mat4[2][3]
        , -mat4[3][0], -mat4[3][1], -mat4[3][2], -mat4[3][3]);
}

////////////////////////////////////////////////////////////////////////////////
// Assignment Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T, typename T1> __forceinline
Mat4<T>& operator*=(Mat4<T>& mat4, T1 scalar)
{
    for (int i = 0; i < mat4.SIZE; ++i)
    {
        for (int j = 0; j < mat4.SIZE; ++j)
        {
            mat4[i][j] *= static_cast<T>(scalar);
        }
    }

    return mat4;
}

template<typename T, typename T1> __forceinline
Mat4<T> &operator+=(Mat4<T>& mat4, const Mat4<T1> &other)
{
    for (int i = 0; i < mat4.SIZE; ++i)
    {
        for (int j = 0; j < mat4.SIZE; ++j)
        {
            mat4[i][j] += static_cast<T>(other[i][j]);
        }
    }

    return mat4;
}

template<typename T, typename T1> __forceinline
Mat4<T> &operator-=(Mat4<T>& mat4, const Mat4<T1> &other)
{
    for (int i = 0; i < mat4.SIZE; ++i)
    {
        for (int j = 0; j < mat4.SIZE; ++j)
        {
            mat4[i][j] -= static_cast<T>(other[i][j]);
        }
    }

    return mat4;
}

template<typename T, typename T1> __forceinline
Mat4<T> &operator*=(Mat4<T>& mat4, const Mat4<T1> &other)
{
    mat4.Clone(Mul(mat4, other));

    return mat4;
}

////////////////////////////////////////////////////////////////////////////////
// Euclidian Space Operators
////////////////////////////////////////////////////////////////////////////////

// TODO
template<typename T> __forceinline
    Mat4<T> Inverse(const Mat4<T>& mat4, T tolerance = 0)
{
    //
    // inv [ A  | b ]  =  [ E  | f ]    A: 3x3, b: 3x1, c': 1x3 d: 1x1
    //     [ c' | d ]     [ g' | h ]
    //
    // If A is invertible use
    //
    //   E  = A^-1 + p*h*r
    //   p  = A^-1 * b
    //   f  = -p * h
    //   g' = -h * c'
    //   h  = 1 / (d - c'*p)
    //   r' = c'*A^-1
    //
    // Otherwise use gauss-jordan elimination
    //

    //
    // We create this alias to ourself so we can easily use own subscript
    // operator.
    /*
    const Mat4<T>& m(*this);

    T m0011 = m[0][0] * m[1][1];
    T m0012 = m[0][0] * m[1][2];
    T m0110 = m[0][1] * m[1][0];
    T m0210 = m[0][2] * m[1][0];
    T m0120 = m[0][1] * m[2][0];
    T m0220 = m[0][2] * m[2][0];

    T detA = m0011 * m[2][2] - m0012 * m[2][1] - m0110 * m[2][2]
        + m0210 * m[2][1] + m0120 * m[1][2] - m0220 * m[1][1];

    bool hasPerspective =
        (!isExactlyEqual(m[0][3], T(0.0)) ||
            !isExactlyEqual(m[1][3], T(0.0)) ||
            !isExactlyEqual(m[2][3], T(0.0)) ||
            !isExactlyEqual(m[3][3], T(1.0)));

    T det;
    if (hasPerspective)
    {
        det = m[0][3] * det3(m, 1, 2, 3, 0, 2, 1)
            + m[1][3] * det3(m, 2, 0, 3, 0, 2, 1)
            + m[2][3] * det3(m, 3, 0, 1, 0, 2, 1)
            + m[3][3] * detA;
    }
    else
    {
        det = detA * m[3][3];
    }

    Mat4<T> inv;
    bool invertible;

    if (isApproxEqual(det, T(0.0), tolerance))
    {
        invertible = false;

    }
    else if (isApproxEqual(detA, T(0.0), T(1e-8)))
    {
        // det is too small to rely on inversion by subblocks
        invertible = m.invert(inv, tolerance);

    }
    else
    {
        invertible = true;
        detA = 1.0 / detA;

        //
        // Calculate A^-1
        //
        inv[0][0] = detA * (m[1][1] * m[2][2] - m[1][2] * m[2][1]);
        inv[0][1] = detA * (-m[0][1] * m[2][2] + m[0][2] * m[2][1]);
        inv[0][2] = detA * (m[0][1] * m[1][2] - m[0][2] * m[1][1]);

        inv[1][0] = detA * (-m[1][0] * m[2][2] + m[1][2] * m[2][0]);
        inv[1][1] = detA * (m[0][0] * m[2][2] - m0220);
        inv[1][2] = detA * (m0210 - m0012);

        inv[2][0] = detA * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
        inv[2][1] = detA * (m0120 - m[0][0] * m[2][1]);
        inv[2][2] = detA * (m0011 - m0110);

        if (hasPerspective)
        {
            //
            // Calculate r, p, and h
            //
            Vec3<T> r;
            r[0] = m[3][0] * inv[0][0] + m[3][1] * inv[1][0]
                + m[3][2] * inv[2][0];
            r[1] = m[3][0] * inv[0][1] + m[3][1] * inv[1][1]
                + m[3][2] * inv[2][1];
            r[2] = m[3][0] * inv[0][2] + m[3][1] * inv[1][2]
                + m[3][2] * inv[2][2];

            Vec3<T> p;
            p[0] = inv[0][0] * m[0][3] + inv[0][1] * m[1][3]
                + inv[0][2] * m[2][3];
            p[1] = inv[1][0] * m[0][3] + inv[1][1] * m[1][3]
                + inv[1][2] * m[2][3];
            p[2] = inv[2][0] * m[0][3] + inv[2][1] * m[1][3]
                + inv[2][2] * m[2][3];

            T h = m[3][3] - p.dot(Vec3<T>(m[3][0], m[3][1], m[3][2]));
            if (isApproxEqual(h, T(0.0), tolerance))
            {
                invertible = false;

            }
            else
            {
                h = 1.0 / h;

                //
                // Calculate h, g, and f
                //
                inv[3][3] = h;
                inv[3][0] = -h * r[0];
                inv[3][1] = -h * r[1];
                inv[3][2] = -h * r[2];

                inv[0][3] = -h * p[0];
                inv[1][3] = -h * p[1];
                inv[2][3] = -h * p[2];

                //
                // Calculate E
                //
                p *= h;
                inv[0][0] += p[0] * r[0];
                inv[0][1] += p[0] * r[1];
                inv[0][2] += p[0] * r[2];
                inv[1][0] += p[1] * r[0];
                inv[1][1] += p[1] * r[1];
                inv[1][2] += p[1] * r[2];
                inv[2][0] += p[2] * r[0];
                inv[2][1] += p[2] * r[1];
                inv[2][2] += p[2] * r[2];
            }
        }
        else
        {
            // Equations are much simpler in the non-perspective case
            inv[3][0] = -(m[3][0] * inv[0][0] + m[3][1] * inv[1][0]
                + m[3][2] * inv[2][0]);
            inv[3][1] = -(m[3][0] * inv[0][1] + m[3][1] * inv[1][1]
                + m[3][2] * inv[2][1]);
            inv[3][2] = -(m[3][0] * inv[0][2] + m[3][1] * inv[1][2]
                + m[3][2] * inv[2][2]);
            inv[0][3] = 0.0;
            inv[1][3] = 0.0;
            inv[2][3] = 0.0;
            inv[3][3] = 1.0;
        }
    }

    if (!invertible) OPENVDB_THROW(ArithmeticError, "Inversion of singular 4x4 matrix");
    */
    return mat4;
}

template<typename T> __forceinline
Mat4<T> Transpose(Mat4<T>& mat4)
{
    return Mat4<T>(mat4[0][0], mat4[1][0], mat4[2][0], mat4[3][0]
        , mat4[0][1], mat4[1][1], mat4[2][1], mat4[3][1]
        , mat4[0][2], mat4[1][2], mat4[2][2], mat4[3][2]
        , mat4[0][3], mat4[1][3], mat4[2][3], mat4[3][3]);
}

template<typename T> __forceinline
Mat4<T> Mul(const Mat4<T>& mat4, const Mat4<T>& mat4_2)
{
    Mat4<T> ret;

    for (int i = 0; i < mat4.SIZE; ++i)
    {
        for (int j = 0; j < mat4.SIZE; ++j)
        {
            for (int k = 0; k < mat4.SIZE; ++k)
            {
                ret[i][j] += mat4[i][k] * static_cast<T>(other[k][j]);
            }
        }
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
// Output Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> inline
std::ostream& operator<<(std::ostream& cout, const Mat4<T>& mat4)
{
    for (int i = 0; i < mat4.SIZE; ++i)
    {
        for (int j = 0; j < mat4.SIZE; ++j)
        {
            if (0 == j)
            {
                cout << "[";
            }
            cout << mat[i][j];
            cout << (mat4.SIZE - 1 != j) ? ", " : "]";
            if (mat4.SIZE - 1 != i)
            {
                cout << endl;
            }
        }
    }

    return cout;
}

////////////////////////////////////////////////////////////////////////////////
// Default template instantiations
////////////////////////////////////////////////////////////////////////////////

typedef Mat4<bool> Mat4x4b;
typedef Mat4<int> Mat4x4i;
typedef Mat4<Float> Mat4x4f;


}
}