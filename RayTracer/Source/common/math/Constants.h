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


template<typename T, typename T1, typename T2> inline
T1 Lerp(T t, T1 v1, T2 v2)
{
    return v1 * (static_cast<T>(1) - t) + static_cast<T1>(v2) * t;
}


template<typename T, typename T1, typename T2> __forceinline
T1 Clamp(T t, T1 min, T2 max)
{
    if (static_cast<T1>(t) < min)
    {
        return static_cast<T1>(min);
    }
    else if (static_cast<T1>(t) > static_cast<T1>(max))
    {
        return static_cast<T1>(max);
    }
    else
    {
        return static_cast<T1>(t);
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


template <typename T> __forceinline
T Mod(T a, T b)
{
    T result = a - (a / b) * b;
    return  static_cast<T>((result < static_cast<T>(0)) ? result + b : result);
}

template <> __forceinline
Float Mod(Float a, Float b)
{
    return std::fmod(a, b);
}

template<typename T> __forceinline
T Log2(T x)
{
    const T invLog2 = static_cast<T>(1.442695040888963387004650940071F);
    return std::log(x) * invLog2;
}

__forceinline
int Log2Int(uint32_t v)
{
    unsigned long lz = 0;
    if (_BitScanReverse(&lz, v))
    {
        return lz;
    }
    return 0;
}

__forceinline
int Log2Int(int32_t v)
{
    return Log2Int(static_cast<uint32_t>(v));
}

inline int Log2Int(uint64_t v)
{
    unsigned long lz = 0;
#ifdef _WIN64
    _BitScanReverse64(&lz, v);
#else
    if (_BitScanReverse(&lz, v >> 32))
    {
        lz += 32;
    }
    else
    {
        _BitScanReverse(&lz, v & 0xffffffff);
    }
#endif // _WIN64
    return lz;
}

inline int Log2Int(int64_t v)
{
    return Log2Int((uint64_t)v);
}

template <typename T> __forceinline
bool IsPowerOf2(T v)
{
    return v && !(v & (v - 1));
}

__forceinline
int32_t RoundUpPow2(int32_t v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return v + 1;
}

__forceinline
int64_t RoundUpPow2(int64_t v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    return v + 1;
}


__forceinline
Float Gamma(int n)
{
    return (static_cast<Float>(n) * MACHINE_EPSILON)
        / (FLOAT_1 - static_cast<Float>(n) * MACHINE_EPSILON);
}

__forceinline
Float GammaCorrect(Float value)
{
    if (value <= static_cast<Float>(0.0031308F))
    {
        return static_cast<Float>(12.92F) * value;
    }

    return static_cast<Float>(1.055F) * std::pow(value, FLOAT_1 / static_cast<Float>(2.4F))
        - static_cast<Float>(0.055F);
}

__forceinline
Float InverseGammaCorrect(Float value)
{
    if (value <= static_cast<Float>(0.04045F))
    {
        return value * FLOAT_1 / static_cast<Float>(12.92F);
    }
    return std::pow((value + static_cast<Float>(0.055F)) * FLOAT_1
        / static_cast<Float>(1.055F), static_cast<Float>(2.4F));
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



inline
Float Erf(Float x)
{
    // constants
    constexpr Float a1 = static_cast<Float>(0.254829592F);
    constexpr Float a2 = static_cast<Float>(-0.284496736F);
    constexpr Float a3 = static_cast<Float>(1.421413741F);
    constexpr Float a4 = static_cast<Float>(-1.453152027F);
    constexpr Float a5 = static_cast<Float>(1.061405429F);
    constexpr Float p = static_cast<Float>(0.3275911F);

    // Save the sign of x
    int sign = x < FLOAT_0 ? 1 : -1;
    x = std::abs(x);

    // A&S formula 7.1.26
    Float t = FLOAT_1 / (FLOAT_1 + p * x);
    Float y = FLOAT_1 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);

    return sign * y;
}

inline
Float ErfInv(Float x)
{
    Float w, p;
    x = Clamp(x, -ONE_MINUS_MACHINE_EPSILON, ONE_MINUS_MACHINE_EPSILON);
    w = -std::log((FLOAT_1 - x) * (FLOAT_1 + x));
    if (w < static_cast<Float>(5.0F))
    {
        w = w - static_cast<Float>(2.5F);
        p = static_cast<Float>(2.81022636e-08F);
        p = static_cast<Float>(3.43273939e-07F) + p * w;
        p = static_cast<Float>(-3.5233877e-06F) + p * w;
        p = static_cast<Float>(-4.39150654e-06F) + p * w;
        p = static_cast<Float>(0.00021858087F) + p * w;
        p = static_cast<Float>(-0.00125372503F) + p * w;
        p = static_cast<Float>(-0.00417768164F) + p * w;
        p = static_cast<Float>(0.246640727F) + p * w;
        p = static_cast<Float>(1.50140941F) + p * w;
    }
    else
    {
        w = std::sqrt(w) - FLOAT_3;
        p = static_cast<Float>(-0.000200214257F);
        p = static_cast<Float>(0.000100950558F) + p * w;
        p = static_cast<Float>(0.00134934322F) + p * w;
        p = static_cast<Float>(-0.00367342844F) + p * w;
        p = static_cast<Float>(0.00573950773F) + p * w;
        p = static_cast<Float>(-0.0076224613F) + p * w;
        p = static_cast<Float>(0.00943887047F) + p * w;
        p = static_cast<Float>(1.00167406F) + p * w;
        p = static_cast<Float>(2.83297682F) + p * w;
    }

    return p * x;
}


}
}