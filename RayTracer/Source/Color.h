#pragma once

#include "pch.h"
#include "common/math/Vec4.h"

namespace Common
{
    template<class T = unsigned char>
    class Color : public Common::Math::Vec4<T>
    {
    public:
        Color() : Color(T(0), T(0), T(0), T(0))
        {}

        Color(const T r, const T g, const T b) : Color(r, g, b, T(0))
        {}

        Color(const T r, const T g, const T b, const T a) : Common::Math::Vec4<T>(r, g, b, a)
        {}

        Color(const Color &c) : Color(c[0], c[1], c[2], c[3])
        {}

        Color& operator=(const Color& c)
        {
            this->Common::Math::Vec4<T>::Clone(c);
            return *this;
        }
    };


    // CColor RGBA default bit = 8888
#define CColor Color<unsigned char>


    const CColor CColor_RED = CColor(255, 0, 0);
    const CColor CColor_GREEN = CColor(0, 255, 0);
    const CColor CColor_BLUE = CColor(0, 0, 255);
    const CColor CColor_BLACK = CColor(0, 0, 0);
    const CColor CColor_WHITE = CColor(255, 255, 255);
}