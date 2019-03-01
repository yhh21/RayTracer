#pragma once

#include "Constants.h"
#include "Vec3.h"

namespace common
{
namespace math
{


template<typename T>
struct Mat3
{
    enum
    {
        SIZE = 3
    };

    T mat[SIZE][SIZE];

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    Mat3()
    {}

    Mat3(T a, T b, T c
        , T d, T e, T f
        , T g, T h, T i)
    {
        mat[0][0] = static_cast<T>(a);
        mat[0][1] = static_cast<T>(b);
        mat[0][2] = static_cast<T>(c);

        mat[1][0] = static_cast<T>(d);
        mat[1][1] = static_cast<T>(e);
        mat[1][2] = static_cast<T>(f);

        mat[2][0] = static_cast<T>(g);
        mat[2][1] = static_cast<T>(h);
        mat[1][2] = static_cast<T>(i);
    }

    Mat3(const Vec3<T1> &v1, const Vec3<T> &v2, const Vec3<T> &v3)
    {
        SetBasis(v1, v2, v3);
    }

    Mat3(T **a)
    {
        CHECK_LT((sizeof(a) / sizeof(*a)), SIZE);
        CHECK_LT(sizeof(*a) / sizeof(**a)), SIZE);

        for (int i = 0; i < SIZE; ++i)
        {
            for (int j = 0; j < SIZE; ++j)
            {
                mat[i][j] = static_cast<T>(a[i][j]);
            }
        }
    }

    Mat3(const Mat3<T> &other)
    {
        Clone(other);
    }


    template<typename T1> __forceinline
        Mat3& operator =(const Mat3<T1>& other)
    {
        Clone(other);
        return *this;
    }

    template<typename T1> __forceinline
        void Clone(const Mat3<T1>& other)
    {
        for (int i = 0; i < SIZE; ++i)
        {
            for (int j = 0; j < SIZE; ++j)
            {
                mat[i][j] = static_cast<T>(other[i][j]);
            }
        }
    }

    void SetBasis(const Vec3<T> &v1, const Vec3<T> &v2, const Vec3<T> &v3)
    {
        for (int i = 0; i < SIZE; ++i)
        {
            mat[0][i] = v1[i];
            mat[1][i] = v2[i];
            mat[2][i] = v3[i];
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
Mat3<T> operator+(const Mat3<T>& mat3) const
{
    return Mat3<T>(mat3);
}

template<typename T> __forceinline
Mat3<T> operator-(const Mat3<T>& mat3) const
{
    return Mat3<T>(-mat3[0][0], -mat3[0][1], -mat3[0][2]
        , -mat3[1][0], -mat3[1][1], -mat3[1][2]
        , -mat3[2][0], -mat3[2][1], -mat3[2][2]);
}

////////////////////////////////////////////////////////////////////////////////
// Assignment Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T, typename T1> __forceinline
Mat3<T>& operator*=(Mat3<T>& mat3, T1 scalar)
{
    for (int i = 0; i < mat3.SIZE; ++i)
    {
        for (int j = 0; j < mat3.SIZE; ++j)
        {
            mat3[i][j] *= static_cast<T>(scalar);
        }
    }

    return mat3;
}

template<typename T, typename T1> __forceinline
Mat3<T> &operator+=(Mat3<T>& mat3, const Mat3<T1> &other)
{
    for (int i = 0; i < mat3.SIZE; ++i)
    {
        for (int j = 0; j < mat3.SIZE; ++j)
        {
            mat3[i][j] += static_cast<T>(other[i][j]);
        }
    }

    return mat3;
}

template<typename T, typename T1> __forceinline
Mat3<T> &operator-=(Mat3<T>& mat3, const Mat3<T1> &other)
{
    for (int i = 0; i < mat3.SIZE; ++i)
    {
        for (int j = 0; j < mat3.SIZE; ++j)
        {
            mat3[i][j] -= static_cast<T>(other[i][j]);
        }
    }

    return mat3;
}

template<typename T, typename T1> __forceinline
Mat3<T> &operator*=(Mat3<T>& mat3, const Mat3<T1> &other)
{
    Mat3<T> ret;

    for (int i = 0; i < ret.SIZE; ++i)
    {
        for (int j = 0; j < ret.SIZE; ++j)
        {
            for (int k = 0; k < ret.SIZE; ++k)
            {
                ret[i][j] += mat3[i][k] * static_cast<T>(other[k][j]);
            }
        }
    }

    mat3.Clone(ret);

    return mat3;
}

////////////////////////////////////////////////////////////////////////////////
// Euclidian Space Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
Mat3<T> Inverse(const Mat3<T>& mat3, T tolerance = 0) const
{
    if (!(static_cast<T>(0) == mat3[0][2] && static_cast<T>(0) == mat3[1][2] && static_cast<T>(0) == mat3[2][2]))
    {
        Mat3<T> ret(mat3[1][1] * mat3[2][2] - mat3[2][1] * mat3[1][2],
            mat3[2][1] * mat3[0][2] - mat3[0][1] * mat3[2][2],
            mat3[0][1] * mat3[1][2] - mat3[1][1] * mat3[0][2],

            mat3[2][0] * mat3[1][2] - mat3[1][0] * mat3[2][2],
            mat3[0][0] * mat3[2][2] - mat3[2][0] * mat3[0][2],
            mat3[1][0] * mat3[0][2] - mat3[0][0] * mat3[1][2],

            mat3[1][0] * mat3[2][1] - mat3[2][0] * mat3[1][1],
            mat3[2][0] * mat3[0][1] - mat3[0][0] * mat3[2][1],
            mat3[0][0] * mat3[1][1] - mat3[1][0] * mat3[0][1]);

        T det = mat3[0][0] * ret[0][0] + mat3[0][1] * ret[1][0] + mat3[0][2] * ret[2][0];
    #ifdef DEBUG
        CHECK_EQ(det, static_cast<T>(0));
    #else
        if (static_cast<T>(0) == det)
        {
            return Mat3<T>();
        }
    #endif // DEBUG

        ret *= static_cast<T>(1) / det;

        return ret;
    }
    else
    {
        Mat3<T> ret(mat3[1][1], -mat3[0][1], static_cast<T>(0)
            , -mat3[1][0], mat3[0][0], static_cast<T>(0)
            , static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));

        T det = mat3[0][0] * mat3[1][1] - mat3[1][0] * mat3[0][1];

    #ifdef DEBUG
        CHECK_EQ(det, static_cast<T>(0));
    #else
        if (static_cast<T>(0) == det)
        {
            return Mat3<T>();
        }
    #endif // DEBUG

        T inv_det = static_cast<T>(1) / det;

        for (int i = 0; i < ret.SIZE - 1; ++i)
        {
            for (int j = 0; j < ret.SIZE - 1; ++j)
            {
                ret[i][j] *= inv_det;
            }
        }

        ret[2][0] = -mat3[2][0] * ret[0][0] - mat3[2][1] * ret[1][0];
        ret[2][1] = -mat3[2][0] * ret[0][1] - mat3[2][1] * ret[1][1];

        return ret;
    }
}

template<typename T> __forceinline
Mat3 Transpose(Mat3<T>& mat3) const
{
    return Mat3<T>(mat3[0][0], mat3[1][0], mat3[2][0]
        , mat3[0][1], mat3[1][1], mat3[2][1]
        , mat3[0][2], mat3[1][2], mat3[2][2]);
}

template<typename T> __forceinline
T Det(Mat3<T>& mat3) const
{
    return mat3[0][0] * mat3[1][1] * mat3[2][2] + mat3[0][1] * mat3[1][2] * mat3[2][0] + mat3[0][2] * mat3[1][0] * mat3[2][1]
        - mat3[0][0] * mat3[1][2] * mat3[2][1] - mat3[0][1] * mat3[1][0] * mat3[2][2] - mat3[0][2] * mat3[1][1] * mat3[2][0];
}

////////////////////////////////////////////////////////////////////////////////
// Output Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> inline
std::ostream& operator<<(std::ostream& cout, const Mat3<T>& mat3)
{
    for (int i = 0; i < mat3.SIZE; ++i)
    {
        for (int j = 0; j < mat3.SIZE; ++j)
        {
            if (0 == j)
            {
                cout << "[";
            }
            cout << mat[i][j];
            cout << (mat3.SIZE - 1 != j) ? ", " : "]";
            if (mat3.SIZE - 1 != i)
            {
                cout << endl;
            }
        }
    }

    return cout;
}


}
}