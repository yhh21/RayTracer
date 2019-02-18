#pragma once

#include "../../ForwardDeclaration.h"
#include <cmath>
#include <algorithm>
#include <vector>

namespace common
{
namespace math
{


static constexpr Float SHADOW_EPSILON = static_cast<Float>(0.0001F);
static constexpr Float MACHINE_EPSILON = std::numeric_limits<Float>::epsilon();
static constexpr Float ONE_MINUS_MACHINE_EPSILON = FLOAT_1 - MACHINE_EPSILON;

static constexpr Float PI = static_cast<Float>(3.14159265358979323846F);
static constexpr Float TWO_PI = FLOAT_2 * PI;
static constexpr Float INV_PI = FLOAT_1 / PI;
static constexpr Float INV_TWO_PI = FLOAT_INV_2 * INV_PI;
static constexpr Float INV_FOUR_PI = FLOAT_INV_2 * FLOAT_INV_2 * INV_PI;
static constexpr Float PI_DIV_TWO = FLOAT_INV_2 * PI;
static constexpr Float PI_DIV_FOUR = FLOAT_INV_2 * FLOAT_INV_2 * PI;
static constexpr Float SQRT_TWO = static_cast<Float>(1.41421356237309504880F);


template<typename T, typename T1, typename T2> __forceinline
T Lerp(T t, T1 v1, T2 v2)
{
    return (static_cast<T>(1) - t) * static_cast<T>(v1) + t * static_cast<T>(v2);
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
T ConvertToRadians(T deg)
{
    return static_cast<T>(PI / static_cast<Float>(180.0F)) * deg;
}

template<typename T> __forceinline
T ConvertToDegrees(T rad)
{
    return static_cast<T>(static_cast<Float>(180.0F) * INV_PI) * rad;
}


__forceinline
Float Gamma(int n)
{
    return (static_cast<Float>(n) * MACHINE_EPSILON)
        / (FLOAT_1 - static_cast<Float>(n) * MACHINE_EPSILON);
}


template <typename Predicate>
int FindInterval(int size, const Predicate &pred)
{
    int first = 0, len = size;
    while (len > 0)
    {
        int half = len >> 1, middle = first + half;
        // Bisect range based on value of _pred_ at _middle_
        if (pred(middle))
        {
            first = middle + 1;
            len -= half + 1;
        }
        else
        {
            len = half;
        }
    }

    return Clamp(first - 1, 0, size - 2);
}


}
}