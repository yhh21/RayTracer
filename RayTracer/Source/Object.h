#pragma once

#include "pch.h"
#include "Color.h"
#include "core/Ray.h"

namespace Objects
{
    class Object
    {
    public:
        Object(const Common::CColor &color)
        {
            this->color = new Common::CColor(color);
        }

        Common::CColor& GetColor() const
        {
            return *color;
        }

        virtual bool Hit(const Core::Ray<Float>& ray, double& tMin) = 0;
    private:
        Common::CColor *color;
    };
}