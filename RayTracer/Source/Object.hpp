#pragma once

#include "pch.h"
#include "Color.hpp"
#include "core/Ray.h"

namespace Objects {
    class Object {
    public:
        Object(const Common::CColor &color) {
            this->color = new Common::CColor(color);
        }

        Common::CColor& GetColor() const { return *color; }

        virtual bool Hit(const Common::Ray<float>& ray, double& tMin) = 0;
    private:
        Common::CColor *color;
    };
}