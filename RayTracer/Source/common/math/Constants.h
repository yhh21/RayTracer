#pragma once

namespace Utilities
{
    namespace Math
    {
        template<typename T> __forceinline
            T Lerp(T t, T v1, T v2)
        {
            return (T(1) - t) * v1 + t * v2;
        }
    }
}