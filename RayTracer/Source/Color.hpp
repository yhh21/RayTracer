#pragma once

#include "pch.h"
#include "Vector.hpp"

namespace Common {
    template<class T = unsigned char>
    class Color : protected Common::Math::Vector4<T> {
    public:
        T GetR() const { return this->GetValue<T>(0); }
        T GetG() const { return this->GetValue<T>(1); }
        T GetB() const { return this->GetValue<T>(2); }
        T GetA() const { return this->GetValue<T>(3); }

        void SetR(const T r) { this->SetValue<T>(0, r); }
        void SetG(const T g) { this->SetValue<T>(1, g); }
        void SetB(const T b) { this->SetValue<T>(2, b); }
        void SetA(const T a) { this->SetValue<T>(3, a); }


        Color() : Color(T(0), T(0), T(0), T(0)) { }

        Color(const T r, const T g, const T b) : Color(r, g, b, T(0)) { }

        Color(const T r, const T g, const T b, const T a) : Common::Math::Vector4<T>(r, g, b, a) { }

        Color(const Color &c) : Color(c.GetR(), c.GetG(), c.GetB(), c.GetA()) { }

        Color& operator=(const Color& c) {
            this->Common::Math::Vector4<T>::Clone(c);
            return *this;
        }

#ifdef DEBUG
        void Print() const {
            string str = "[" + to_string((int)this->GetR()) + ", " + to_string((int)this->GetG()) + ", " +
                to_string((int)this->GetB()) + ", " + to_string((int)this->GetA()) + "]\n";
            Common::DebugTools::PrintDebugLog(str);
        }
#endif // DEBUG
    };


    // CColor RGBA default bit = 8888
#define CColor Color<unsigned char>


    const CColor CColor_RED = CColor(255, 0, 0);
    const CColor CColor_GREEN = CColor(0, 255, 0);
    const CColor CColor_BLUE = CColor(0, 0, 255);
    const CColor CColor_BLACK = CColor(0, 0, 0);
    const CColor CColor_WHITE = CColor(255, 255, 255);
}