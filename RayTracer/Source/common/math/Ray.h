#pragma once

#include "Constants.h"
#include "Vec3.h"

class Medium;

namespace common
{
namespace math
{


template<typename T>
class Ray
{
public:
    Vec3<T> origin, dir;
    T time;
    mutable T t_min, t_max;

    /// TODO
    const Medium *medium;


    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    Ray() : t_max(std::numeric_limits<T>::infinity()), t_min(static_cast<T>(0))
        , time(static_cast<T>(0)), medium(nullptr)
    {}

    Ray(const Vec3<T>& origin = Vec3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0))
        , const Vec3<T>& direction = Vec3<T>(static_cast<T>(0), static_cast<T>(0), static_cast<T>(1))
        , T t_max = (std::numeric_limits<T>::max)(), T t_min = static_cast<T>(0)
        , T time = static_cast<T>(0))
        : origin(origin), dir(direction), t_max(t_max), t_min(t_min)
    {}


    __forceinline
        Vec3<T> operator()(T t) const
    {
        return origin + dir * t;
    }
};


}
}