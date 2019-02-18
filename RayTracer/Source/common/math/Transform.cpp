#include "Transform.h"

#include "Bounds3.h"
#include "Ray.h"
#include "RayDifferential.h"
#include "../../Core/interaction/SurfaceInteraction.h"

namespace common
{
namespace math
{


template<typename T>
Ray<T> Transform<T>::operator()(const Ray<T> &ray) const
{
    Vec3<T> origin_error;
    Vec3<T> origin = (*this)(ray.origin, &origin_error);
    Vec3<T> dir = (*this)(ray.dir);

    // Offset ray origin to edge of error bounds and compute _tMax_
    T length_squared = LengthSquared(dir);
    T t_max = ray.t_max;
    if (length_squared > static_cast<T>(0))
    {
        T dt = Dot(Abs(dir), origin_error) / length_squared;
        origin += dir * dt;
        t_max -= dt;
    }

    return Ray(origin, dir, t_max, ray.t_min, ray.time, ray.medium);
}

template<typename T>
RayDifferential<T> Transform<T>::operator()(const RayDifferential<T> &ray_d) const
{
    RayDifferential<T> ret((*this)(static_cast<Ray<T>>(ray_d)));

    ret.has_differentials = ray_d.has_differentials;
    ret.rx_origin = (*this)(ray_d.rx_origin);
    ret.ry_origin = (*this)(ray_d.ry_origin);
    ret.rx_direction = (*this)(ray_d.rx_direction);
    ret.ry_direction = (*this)(ray_d.ry_direction);

    return ret;
}

template<typename T>
Bounds3<T> Transform<T>::operator()(const Bounds3<T> &bounds) const
{
    Bounds3<T> ret((*this)(Vec3<T>(bounds.point_min.x, bounds.point_min.y, bounds.point_min.z)));

    ret = Union(ret, (*this)(Vec3<T>(bounds.point_max.x, bounds.point_min.y, bounds.point_min.z)));
    ret = Union(ret, (*this)(Vec3<T>(bounds.point_min.x, bounds.point_max.y, bounds.point_min.z)));
    ret = Union(ret, (*this)(Vec3<T>(bounds.point_min.x, bounds.point_min.y, bounds.point_max.z)));
    ret = Union(ret, (*this)(Vec3<T>(bounds.point_min.x, bounds.point_max.y, bounds.point_max.z)));
    ret = Union(ret, (*this)(Vec3<T>(bounds.point_max.x, bounds.point_max.y, bounds.point_min.z)));
    ret = Union(ret, (*this)(Vec3<T>(bounds.point_max.x, bounds.point_min.y, bounds.point_max.z)));
    ret = Union(ret, (*this)(Vec3<T>(bounds.point_max.x, bounds.point_max.y, bounds.point_max.z)));

    return ret;
}

template<typename T>
core::interaction::SurfaceInteraction Transform<T>::operator()(const core::interaction::SurfaceInteraction &si) const
{
    core::interaction::SurfaceInteraction ret;
    // Transform _p_ and _pError_ in _SurfaceInteraction_
    ret.p = (*this)(si.p, si.p_error, &ret.p_error);

    // Transform remaining members of _SurfaceInteraction_
    ret.n = Normalize((*this)(si.n));
    ret.wo = Normalize((*this)(si.wo));
    ret.time = si.time;
    ret.medium_interface = si.medium_interface;
    ret.uv = si.uv;

    ret.shape = si.shape;
    ret.dp_du = (*this)(si.dp_du);
    ret.dp_dv = (*this)(si.dp_dv);
    ret.dn_du = (*this)(si.dn_du);
    ret.dn_dv = (*this)(si.dn_dv);

    ret.shading.n = Normalize((*this)(si.shading.n));
    ret.shading.dp_du = (*this)(si.shading.dp_du);
    ret.shading.dp_dv = (*this)(si.shading.dp_dv);
    ret.shading.dn_du = (*this)(si.shading.dn_du);
    ret.shading.dn_dv = (*this)(si.shading.dn_dv);

    ret.du_dx = si.du_dx;
    ret.dv_dx = si.dv_dx;
    ret.du_dy = si.du_dy;
    ret.dv_dy = si.dv_dy;
    ret.dp_dx = (*this)(si.dp_dx);
    ret.dp_dy = (*this)(si.dp_dy);

    //ret.bsdf = si.bsdf;
    //ret.bssrdf = si.bssrdf;
    //ret.primitive = si.primitive;

    //ret.n = Faceforward(ret.n, ret.shading.n);

    ret.shading.n = Faceforward(ret.shading.n, ret.n);
    ret.face_index = si.face_index;

    return ret;
}


}
}