#include "Transform.h"
#include "../../Core/interaction/SurfaceInteraction.h"

namespace common
{
namespace math
{


template<typename T>
core::interaction::SurfaceInteraction Transform<T>::operator()(const core::interaction::SurfaceInteraction &si) const
{
    core::interaction::SurfaceInteraction ret;
    /// Transform _p_ and _pError_ in _SurfaceInteraction_
    ret.p = (*this)(si.p, si.p_error, &ret.p_error);

    /// Transform remaining members of _SurfaceInteraction_
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