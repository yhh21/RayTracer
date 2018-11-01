#pragma once

#include "pch.h"
#include "Vector.hpp"
#include "Point.hpp"

using namespace Utilities::Math;

namespace Utilities {
    class Ray {
    public:
        Ray(const DPoint3 &origin, const DVector3 &direction) {
            this->origin = new DPoint3(origin);
            this->direction = new DVector3(direction);
            //this->direction->Normalize();
        }

        const DPoint3& GetOrigin() const { return *(this->origin); }
        const DVector3& GetDirection() const { return *(this->direction); }
    private:
        DPoint3 *origin;
        DVector3 *direction;
    };
}