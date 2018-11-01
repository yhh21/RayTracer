#pragma once

#include "Vector.hpp"

namespace Utilities {
    namespace Math {
#define Point Vector
#define Point2 Vector2
#define Point3 Vector3
#define Point4 Vector4

#define DPoint Point<double>
#define DPoint2 Point2<double>
#define DPoint3 Point3<double>
#define DPoint4 Point4<double>
    }
}