#pragma once

#include "pch.h"
#include <cmath>

namespace Common
{
    namespace Math
    {
        static constexpr Float SHADOW_EPSILON = static_cast<Float>(0.0001F);
        static constexpr Float PI = static_cast<Float>(3.14159265358979323846F);
        static constexpr Float TWO_PI = static_cast<Float>(2.0F) * PI;
        static constexpr Float INV_PI = static_cast<Float>(1.0F) / PI;
        static constexpr Float INV_TWO_PI = static_cast<Float>(0.5F) * INV_PI;
        static constexpr Float INV_FOUR_PI = static_cast<Float>(0.25F) * INV_PI;
        static constexpr Float PI_DIV_TWO = static_cast<Float>(0.5F) * PI;
        static constexpr Float PI_DIV_FOUR = static_cast<Float>(0.25F) * PI;
        static constexpr Float SQRT_TWO = static_cast<Float>(1.41421356237309504880F);


        template<typename T> __forceinline
            T Lerp(T t, T v1, T v2)
        {
            return (static_cast<T>(1) - t) * v1 + t * v2;
        }

        template<typename T, typename T1, typename T2> __forceinline
            T Clamp(T t, T1 min, T2 max)
        {
            if (t < static_cast<T>(min))
            {
                return static_cast<T>(min);
            }
            else if (t > static_cast<T>(max))
            {
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
            if (t == static_cast<T>(0))
            {
                int i = 0;
                ++i;
            }
            CHECK_NE(t, static_cast<T>(0));
            return static_cast<T>(1) / t;
        }

        template<typename T> __forceinline
            T rsqrt(T t)
        {
            CHECK_GE(t, static_cast<T>(0));
            return static_cast<T>(1) / sqrt(t);
        }


        template<typename T> __forceinline
            T Radians(T deg)
        {
            return static_cast<T>(PI / 180.0F) * deg;
        }

        template<typename T> __forceinline
            T Degrees(T rad)
        {
            return static_cast<T>(180.0F / PI) * rad;
        }
    }
}