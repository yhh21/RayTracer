#pragma once

#include "Constants.h"
#include "Vec3.h"

namespace Common
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

            Mat3()
            {}

            template<typename T1>
            Mat3(T1 a, T1 b, T1 c
                , T1 d , T1 e, T1 f
                , T1 g, T1 h , T1 i)
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

            template<typename T1>
            Mat3(const Vec3<T1> &v1, const Vec3<T1> &v2, const Vec3<T1> &v3)
            {
                SetBasis(v1, v2, v3);
            }

            template<typename T1>
            Mat3(T1 **a)
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
                CHECK_LT(index, SIZE);
                return &(mat[index]);
            }

            __forceinline
                T* operator[](size_t i)
            {
                CHECK_LT(index, SIZE);
                return &(mat[index]);
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

        /// TODO
        template<typename T, typename T1> __forceinline
            Mat3<T> &operator*=(Mat3<T>& mat3, const Mat3<T1> &other)
        {
            T ret[mat3.SIZE][mat3.SIZE];

            for (int i = 0; i < mat3.SIZE; ++i)
            {
                for (int j = 0; j < mat3.SIZE; ++j)
                {
                    for (int k = 0; k < mat3.SIZE; ++k)
                    {
                        ret += mat3[i][k] * static_cast<T>(other[k][j]);
                    }
                }
            }

            mat3 = ret;

            return mat3;
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// Euclidian Space Operators
        ////////////////////////////////////////////////////////////////////////////////

        /// TODO
        template<typename T> __forceinline
            Mat3 Inverse(Mat3<T>& mat3, T tolerance = 0) const
        {
            CHECK_NE(Det(mat3), static_cast<T>(0));




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
            const Mat3<T>& m(*this);

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

            Mat3<T> inv;
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
            return inv;
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
        /// Output Operators
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

        ////////////////////////////////////////////////////////////////////////////////
        /// Default template instantiations
        ////////////////////////////////////////////////////////////////////////////////

        typedef Mat3<bool> Mat3x3b;
        typedef Mat3<int> Mat3x3i;
        typedef Mat3<Float> Mat3x3f;
    }
}