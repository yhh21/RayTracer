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
            this->r = c.GetR();
            this->g = c.GetG();
            this->b = c.GetB();
            this->a = c.GetA();
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
}