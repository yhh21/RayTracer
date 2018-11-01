#pragma once

#include "pch.h"

namespace Utilities {
    // RGBA default bit = 8888
    template<class T = unsigned char>
    class Color {
    public:
        T GetR() const { return r; }
        T GetG() const { return g; }
        T GetB() const { return b; }
        T GetA() const { return a; }

        void SetR(const T r) { this->r = r; }
        void SetG(const T g) { this->g = g; }
        void SetB(const T b) { this->b = b; }
        void SetA(const T a) { this->a = a; }


        Color() : Color(T(0), T(0), T(0), T(0)) { }

        Color(const T r, const T g, const T b) : Color(r, g, b, T(0)) { }

        Color(const T r, const T g, const T b, const T a) {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }

        Color(const Color &c) : Color(c.GetR(), c.GetG(), c.GetB(), c.GetA()) { }

        Color& operator=(const Color& c) {
            Clone(c);
            return *this;
        }


        template<class T2>
        void Clone(const Color<T2> &c) {
            this->r = static_cast<T>(c.GetR());
            this->g = static_cast<T>(c.GetG());
            this->b = static_cast<T>(c.GetB());
            this->a = static_cast<T>(c.GetA());
        }

#ifdef DEBUG
        void Print() const {
            cout << (int)r << " " << (int)g << " " << (int)b << " " << (int)a << "\n";
        }
#endif // DEBUG
    private:
        T r, g, b, a;
    };


#define CColor Color<unsigned char>


    const CColor CColor_RED = CColor(255, 0, 0);
    const CColor CColor_GREEN = CColor(0, 255, 0);
    const CColor CColor_BLUE = CColor(0, 0, 255);
    const CColor CColor_BLACK = CColor(0, 0, 0);
    const CColor CColor_WHITE = CColor(255, 255, 255);
}