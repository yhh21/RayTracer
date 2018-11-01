#pragma once

#include "pch.h"
#include "Color.hpp"
#include "Ray.hpp"

namespace Objects {
    class Object {
    public:
        Object(const Utilities::CColor &color) {
            this->color = new Utilities::CColor(color);
        }

        Utilities::CColor& GetColor() const { return *color; }

        virtual bool Hit(const Utilities::Ray& ray, double& tMin) = 0;
    private:
        Utilities::CColor *color;
    };
}