#pragma once

#include "pch.h"
#include "Object.hpp"
#include "Point.hpp"

using namespace Utilities;
using namespace Utilities::Math;

namespace Objects {
    class Sphere : public Object {
    public:
        Sphere(const CColor &color, const DPoint3 &center, const double radius) : Object(color) {
            this->center.Clone(center);
            this->radius = radius;
        }

        virtual bool Hit(const Ray& ray, double& tMin) {
#ifdef DEBUG
            ray.GetOrigin().Print();
#endif // DEBUG
            DVector oc = (ray.GetOrigin() - center);
#ifdef DEBUG
            oc.Print();
#endif // DEBUG
            double a = ray.GetDirection().Dot(ray.GetDirection());
            double b = oc.Dot(ray.GetDirection()) * 2.0;
            double c = oc.Dot(oc) - radius * radius;

            double discriminant = b * b - 4.0 * a * c;

            if (discriminant >= 0.0) {
                return true;
            }

            return false;
        }
    private:
        DPoint3 center;
        double radius;
    };
}