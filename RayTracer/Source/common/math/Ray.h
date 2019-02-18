#pragma once

#include "Constants.h"
#include "Vec3.h"
#include "../../core/interaction/Medium.h"

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

    const core::interaction::Medium *medium;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    Ray()
    {}

    Ray(const Vec3<T>& origin, const Vec3<T>& direction
        , T t_max = (std::numeric_limits<T>::max)(), T t_min = static_cast<T>(0)
        , T time = static_cast<T>(0), const core::interaction::Medium *medium = nullptr)
        : origin(origin), dir(direction), t_max(t_max), t_min(t_min), time(time), medium(medium)
    {}


    __forceinline
        Vec3<T> operator()(T t) const
    {
        return origin + dir * t;
    }
};


// TODO
template <typename T>
inline Vec3<T> OffsetRayOrigin(const Vec3<T> &p, const Vec3<T> &p_error,
    const Vec3<T> &n, const Vec3<T> &w)
{
    T d = Dot(Abs(n), p_error);
#ifdef FLOAT_AS_DOUBLE
    // We have tons of precision; for now bump up the offset a bunch just
    // to be extra sure that we start on the right side of the surface
    // (In case of any bugs in the epsilons code...)
    d *= static_cast<T>(1024.0f);
#endif
    Vec3<T> offset = d * Vec3<T>(n);
    if (Dot(w, n) < static_cast<T>(0))
    {
        offset = -offset;
    }
    Vec3<T> po = p + offset;
    /* TODO
    // Round offset point _po_ away from _p_
    for (int i = 0; i < 3; ++i)
    {
        if (offset[i] > static_cast<T>(0))
        {
            po[i] = NextFloatUp(po[i]);
        }
        else if (offset[i] < 0)
        {
            po[i] = NextFloatDown(po[i]);
        }
    }
    */

    return po;
}


}
}