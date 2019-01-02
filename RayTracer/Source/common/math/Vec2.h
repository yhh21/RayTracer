#pragma once

#include <cmath>

namespace Common
{
    namespace Math
    {
        template<typename T>
        struct Vec2
        {
            T x, y;

            static const size_t SIZE = 2;

            ////////////////////////////////////////////////////////////////////////////////
            /// Construction
            ////////////////////////////////////////////////////////////////////////////////

            __forceinline
                Vec2() {}

            __forceinline explicit
                Vec2(const T& a) : x(a), y(a) {}

            __forceinline explicit
                Vec2(const T& x, const T& y) : x(x), y(y) {}


            __forceinline
                Vec2(const Vec2& other)
            {
                Clone(other);
            }

            template<typename T1> __forceinline
                Vec2(const Vec2<T1>& a) : x(static_cast<T>(a.x)), y(static_cast<T>(a.y)) {}

            template<typename T1> __forceinline
                Vec2& operator =(const Vec2<T1>& other)
            {
                Clone(other);
                return *this;
            }


            template<typename T1> __forceinline
                void Clone(const Vec3<T1>& other)
            {
                x = other.x; y = other.y;
            }


            __forceinline
                const T& operator [](const size_t axis) const
            {
                assert(axis < SIZE);
                return (&x)[axis];
            }

            __forceinline
                T& operator [](const size_t axis)
            {
                assert(axis < SIZE);
                return (&x)[axis];
            }
        };


        ////////////////////////////////////////////////////////////////////////////////
        /// Unary Operators
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
            return Vec2<T>(abs(a.x), abs(a.y));
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
            return Vec2<T>(sqrt(a.x), sqrt(a.y));
        }


        ////////////////////////////////////////////////////////////////////////////////
        /// Euclidian Space Operators
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
        /// Binary Operators
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
        /// Assignment Operators
        ////////////////////////////////////////////////////////////////////////////////

        template<typename T> __forceinline
            Vec2<T>& operator +=(Vec2<T>& a, const Vec2<T>& b)
        {
            a.x += b.x; a.y += b.y;
            return a;
        }

        template<typename T> __forceinline
            Vec2<T>& operator -=(Vec2<T>& a, const Vec2<T>& b)
        {
            a.x -= b.x; a.y -= b.y;
            return a;
        }

        template<typename T> __forceinline
            Vec2<T>& operator *=(Vec2<T>& a, const T& b)
        {
            a.x *= b; a.y *= b;
            return a;
        }

        template<typename T> __forceinline
            Vec2<T>& operator /=(Vec2<T>& a, const T& b)
        {
            a.x /= b; a.y /= b;
            return a;
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// Comparison Operators
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
            if (a.x != b.x) return a.x < b.x;
            if (a.y != b.y) return a.y < b.y;
            return false;
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// Select
        ////////////////////////////////////////////////////////////////////////////////

        template<typename T> __forceinline
            int MaxDim(const Vec2<T>& a)
        {
            if (a.x > a.y) return 0;
            else return 1;
        }

        template<typename T> __forceinline
            int MinDim(const Vec2<T>& a)
        {
            if (a.x <= a.y) return 0;
            else return 1;
        }

        template <typename T> __forceinline
            Vec2<T> Permute(const Vec2<T> &a, size_t x, size_t y)
        {
            return Vec2<T>(a[x], a[y]);
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// Output Operators
        ////////////////////////////////////////////////////////////////////////////////

        template<typename T> inline
            std::ostream& operator<<(std::ostream& cout, const Vec2<T>& a)
        {
            return cout << "(" << a.x << ", " << a.y << ")";
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// Default template instantiations
        ////////////////////////////////////////////////////////////////////////////////

        typedef Vec2<bool> Vec2b;
        typedef Vec2<int> Vec2i;
        typedef Vec2<float> Vec2f;
    }
}