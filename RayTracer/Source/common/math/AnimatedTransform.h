#pragma once

#include "../../ForwardDeclaration.h"
#include "Constants.h"
#include "Vec3.h"
#include "Quaternion.h"
#include "Mat4.h"

namespace common
{
namespace math
{

template<typename T>
class AnimatedTransform
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    AnimatedTransform(const Transform<T> *p_start_transform, const T &start_time
        , const Transform<T> *p_end_transform, const T &end_time);


    static void Decompose(const Mat4<T> &m, Vec3<T> *translation, Quaternion<T> *rotation_quat, Mat4<T> *scale);

    void Interpolate(T time, Transform<T> *t) const;

    bool HasScale() const
    {
        return p_start_transform->HasScale() || p_end_transform->HasScale();
    }


    Ray<T> operator()(const Ray<T> &r) const;

    RayDifferential<T> operator()(const RayDifferential<T> &r) const;

    Vec3<T> operator()(T time, const Vec3<T> &v) const;


    Bounds3<T> MotionBounds(const Bounds3<T> &b) const;

    Bounds3<T> BoundPointMotion(const Vec3<T> &p) const;

private:

    const Transform<T> *p_start_transform, *p_end_transform;
    const bool is_actually_animated;
    const T start_time, end_time;

    std::vector<Vec3<T>> translations;
    std::vector<Quaternion<T>> rotations;
    std::vector<Mat4<T>> scales;

    bool has_rotation;


    struct DerivativeTerm
    {
        DerivativeTerm()
        {}

        DerivativeTerm(T c, T x, T y, T z)
            : kc(c), kx(x), ky(y), kz(z)
        {}

        T kc, kx, ky, kz;

        T Eval(const Vec3<T> &p) const
        {
            return kc + kx * p.x + ky * p.y + kz * p.z;
        }
    };

    DerivativeTerm c1[3], c2[3], c3[3], c4[3], c5[3];
};


}
}