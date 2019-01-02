#pragma once

#include <cmath>

namespace Common
{
    namespace Math
    {
        static constexpr float SHADOW_EPSILON = 0.0001F;
        static constexpr float PI = 3.14159265358979323846;
        static constexpr float TWO_PI = 2.0F * PI;
        static constexpr float INV_PI = 1.0F / PI;
        static constexpr float INV_TWO_PI = 1 / TWO_PI;
        static constexpr float INV_FOUR_PI = INV_TWO_PI * INV_TWO_PI;
        static constexpr float PI_DIV_TWO = PI * 0.5F;
        static constexpr float PI_DIV_FOUR = PI * 025F;
        static constexpr float SQRT_TWO = 1.41421356237309504880;


        template<typename T> __forceinline
            T Lerp(T t, T v1, T v2)
        {
            return (static_cast<T>(1) - t) * v1 + t * v2;
        }

        template<typename T, typename T1, typename T2> __forceinline
            T Clamp(T t, T1 min, T2 max)
        {
            if (t < static_cast<T>(min)) {
                return static_cast<T>(min);
            }
            else if (t > static_cast<T>(max)) {
                return static_cast<T>(max);
            }
            else
            {
                return t;
            }
        }


        template<typename T> __forceinline
            T rcp(T t)
        {
            assert(t != static_cast<T>(0));
            return static_cast<T>(1) / t;
        }

        template<typename T> __forceinline
            T rsqrt(T t)
        {
            assert(t >= static_cast<T>(0));
            return static_cast<T>(1) / sqrt(t);
        }


        template<typename T> __forceinline
        T Radians(T deg)
        { return static_cast<T>(PI / 180.0F) * deg; }

        template<typename T> __forceinline
        T Degrees(T rad)
        { return static_cast<T>(180.0F / PI) * rad; }
    }
}