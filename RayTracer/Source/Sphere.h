#pragma once

#include "pch.h"
#include "common/math/Constants.h"
#include "Object.h"
#include "common/math/Vec3.h"

using namespace Common;
using namespace Common::Math;

namespace Objects
{
    class Sphere : public Object
    {
    public:
        Vec3f& GetCenter() const
        {
            return *(this->center);
        }
        double GetRadius() const
        {
            return this->radius;
        }

        Sphere(const CColor &color, const Vec3f &center, const double radius) : Object(color)
        {
            this->center = new Vec3f(center);
            this->radius = radius;
        }

        virtual bool Hit(const Core::Ray<Float>& ray, double& tMin)
        {
            Vec3f oc = (ray.origin - GetCenter());
            double a = LengthSquared(ray.dir);
            double b = Dot(oc, ray.dir) * 2.0;
            double c = LengthSquared(oc) - GetRadius() * GetRadius();

            double discriminant = b * b - 4.0 * a * c;

            if (discriminant >= 0.0)
            {
                double e = sqrt(discriminant);
                double t = (-b - e) / (2 * a);

                if (t <= 0)
                {
                    t = (-b + e) / (2 * a);
                }

                if (t > 0)
                {
                    tMin = t;
                    return true;
                }
            }

            return false;
        }
    private:
        Vec3f *center;
        double radius;
    };
}