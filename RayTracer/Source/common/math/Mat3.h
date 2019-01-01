#pragma once

#include <cmath>
#include "Vec3.h"

namespace Utilities
{
    namespace Math
    {
        template<typename T>
        struct Mat3
        {
            T mat[SIZE][SIZE];

            static const size_t SIZE = 3;


            ////////////////////////////////////////////////////////////////////////////////
            /// Construction
            ////////////////////////////////////////////////////////////////////////////////

            Mat3() {}

            template<typename T1>
            Mat3(T1 a, T1 b, T1 c
                , T1 d, T1 e, T1 f
                , T1 g, T1 h, T1 i)
            {
                mat[0][0] = static_cast<T>(a);
                mat[0][1] = static_cast<T>(b);
                mat[0][2] = static_cast<T>(c);
                mat[1][0] = static_cast<T>(d);
                mat[1][1] = static_cast<T>(e);
                mat[1][2] = static_cast<T>(f);
                mat[2][0] = static_cast<T>(g);
                mat[2][1] = static_cast<T>(h);
                mat[2][2] = static_cast<T>(i);
            }

            template<typename T1>
            Mat3(const Vec3<T1> &v1, const Vec3<T1> &v2, const Vec3<T1> &v3)
            {
                SetBasis(v1, v2, v3);
            }

            template<typename T1>
            Mat3(T1 **a)
            {
                assert((sizeof(a) / sizeof(*a)) < SIZE && (sizeof(*a) / sizeof(**a)) < SIZE);
                mat[0][0] = static_cast<T>(a[0][0]);
                mat[0][1] = static_cast<T>(a[0][1]);
                mat[0][2] = static_cast<T>(a[0][2]);
                mat[1][0] = static_cast<T>(a[1][0]);
                mat[1][1] = static_cast<T>(a[1][1]);
                mat[1][2] = static_cast<T>(a[1][2]);
                mat[2][0] = static_cast<T>(a[2][0]);
                mat[2][1] = static_cast<T>(a[2][1]);
                mat[2][2] = static_cast<T>(a[2][2]);
            }

            Mat3(const Mat3<T> &other)
            {
                Clone(other);
            }

            template<typename T1> explicit
                Mat3(const Mat3<T1> &other)
            {
                Clone(other);
            }

            template<typename T1> __forceinline
                Mat3& operator =(const Mat3<T1>& other)
            {
                Clone(other);
                return *this;
            }

            explicit Mat3(const Mat4<T> &other)
            {
                for (int i = 0; i < SIZE; ++i)
                {
                    for (int j = 0; j < SIZE; ++j)
                    {
                        mat[i][j] = other[i][j];
                    }
                }
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
                mat[0][0] = v1[0];
                mat[0][1] = v1[1];
                mat[0][2] = v1[2];
                mat[1][0] = v2[0];
                mat[1][1] = v2[1];
                mat[1][2] = v2[2];
                mat[2][0] = v3[0];
                mat[2][1] = v3[1];
                mat[2][2] = v3[2];
            }

            /*
            __forceinline
                const T& operator[](size_t index) const
            {
                assert(index < SIZE * SIZE);
                return mat[index / SIZE][j % SIZE];
            }

            __forceinline
                T& operator[](size_t index)
            {
                assert(index < SIZE * SIZE);
                return mat[index / SIZE][j % SIZE];
            }
            */

            __forceinline
                const T& operator()(size_t i, size_t j) const
            {
                assert(i < SIZE && j < SIZE);
                return mat[i][j];
            }

            __forceinline
                T& operator()(size_t i, size_t j)
            {
                assert(i < SIZE && j < SIZE);
                return mat[i][j];
            }
        }


        ////////////////////////////////////////////////////////////////////////////////
        /// Unary Operators
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
        /// Assignment Operators
        ////////////////////////////////////////////////////////////////////////////////

        template<typename T, typename T1> __forceinline
            Mat3<T>& operator*=(const Mat3<T>& mat3, T1 scalar)
        {
            mat3[0][0] *= static_cast<T>(scalar);
            mat3[0][1] *= static_cast<T>(scalar);
            mat3[0][2] *= static_cast<T>(scalar);
            mat3[1][0] *= static_cast<T>(scalar);
            mat3[1][1] *= static_cast<T>(scalar);
            mat3[1][2] *= static_cast<T>(scalar);
            mat3[2][0] *= static_cast<T>(scalar);
            mat3[2][1] *= static_cast<T>(scalar);
            mat3[2][2] *= static_cast<T>(scalar);
            return *this;
        }

        template<typename T, typename T1> __forceinline
            Mat3<T> &operator+=(const Mat3<T>& mat3, const Mat3<T1> &other)
        {
            mat3[0][0] += static_cast<T>(other[0][0]);
            mat3[0][1] += static_cast<T>(other[0][1]);
            mat3[0][2] += static_cast<T>(other[0][2]);
            mat3[1][3] += static_cast<T>(other[1][0]);
            mat3[1][4] += static_cast<T>(other[1][1]);
            mat3[1][5] += static_cast<T>(other[1][2]);
            mat3[2][6] += static_cast<T>(other[2][0]);
            mat3[2][7] += static_cast<T>(other[2][1]);
            mat3[2][8] += static_cast<T>(other[2][2]);
            return *this;
        }

        template<typename T, typename T1> __forceinline
            Mat3<T> &operator-=(const Mat3<T>& mat3, const Mat3<T1> &other)
        {
            mat3[0][0] -= static_cast<T>(other[0][0]);
            mat3[0][1] -= static_cast<T>(other[0][1]);
            mat3[0][2] -= static_cast<T>(other[0][2]);
            mat3[1][3] -= static_cast<T>(other[1][0]);
            mat3[1][4] -= static_cast<T>(other[1][1]);
            mat3[1][5] -= static_cast<T>(other[1][2]);
            mat3[2][6] -= static_cast<T>(other[2][0]);
            mat3[2][7] -= static_cast<T>(other[2][1]);
            mat3[2][8] -= static_cast<T>(other[2][2]);
            return *this;
        }

        template<typename T, typename T1> __forceinline
            Mat3<T> &operator*=(const Mat3<T>& mat3, const Mat3<T1> &other)
        {
            mat3[0][0] = static_cast<T>(other[0][0] * other[0][0]
                + other[0][1] * other[1][3] + other[0][2] * other[2][6]);
            mat3[0][1] = static_cast<T>(other[0][0] * other[0][1]
                + other[0][1] * other[1][4] + other[0][2] * other[2][7]);
            mat3[0][2] = static_cast<T>(other[0][0] * other[0][2]
                + other[0][1] * other[1][5] + other[0][2] * other[2][8]);

            mat3[1][3] = static_cast<T>(other[1][3] * other[0][0]
                + other[1][4] * other[1][3] + other[1][5] * other[2][6]);
            mat3[1][4] = static_cast<T>(other[1][3] * other[0][1]
                + other[1][4] * other[1][4] + other[1][5] * other[2][7]);
            mat3[1][5] = static_cast<T>(other[1][3] * other[0][2]
                + other[1][4] * other[1][5] + other[1][5] * other[2][8]);

            mat3[2][6] = static_cast<T>(other[2][6] * other[0][0]
                + other[2][7] * other[1][3] + other[2][8] * other[2][6]);
            mat3[2][7] = static_cast<T>(other[2][6] * other[0][1]
                + other[2][7] * other[1][4] + other[2][8] * other[2][7]);
            mat3[2][8] = static_cast<T>(other[2][6] * other[0][2]
                + other[2][7] * other[1][5] + other[2][8] * other[2][8]);

            return *this;
        }
    }
}