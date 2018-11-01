#pragma once

#include "pch.h"
#include "Object.hpp"
#include "Point.hpp"
#include <cmath>

using namespace Utilities;
using namespace Utilities::Math;

namespace Objects {
    class Sphere : public Object {
    public:
        DPoint3& GetCenter() const { return *(this->center); }
        double GetRadius() const { return this->radius; }

        Sphere(const CColor &color, const DPoint3 &center, const double radius) : Object(color) {
            this->center = new DPoint3(center);
            this->radius = radius;
        }

        virtual bool Hit(const Ray& ray, double& tMin) {
            DVector oc = (ray.GetOrigin() - GetCenter());
            double a = ray.GetDirection().Dot(ray.GetDirection());
            double b = oc.Dot(ray.GetDirection()) * 2.0;
            double c = oc.Dot(oc) - GetRadius() * GetRadius();

            double discriminant = b * b - 4.0 * a * c;

            if (discriminant >= 0.0) {
                double e = sqrt(discriminant);
                double t = (-b - e) / (2 * a);

                if (t <= 0) {
                    t = (-b + e) / (2 * a);
                }

                if (t > 0) {
                    tMin = t;
                    return true;
                }
            }

            return false;
        }
    private:
        DPoint3 *center;
        double radius;
    };
}