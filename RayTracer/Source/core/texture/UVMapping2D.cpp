#include "UVMapping2D.h"
#include "../../common/math/Vec2.h"
#include "../interaction/SurfaceInteraction.h"

namespace core
{
namespace texture
{


UVMapping2D::UVMapping2D(Float su, Float sv, Float du, Float dv)
    : su(su), sv(sv), du(du), dv(dv)
{}

common::math::Vec2f UVMapping2D::Map(const interaction::SurfaceInteraction &si, common::math::Vec2f *dstdx,
    common::math::Vec2f *dstdy) const
{
    // Compute texture differentials for 2D identity mapping
    *dstdx = common::math::Vec2f(su * si.du_dx, sv * si.dv_dx);
    *dstdy = common::math::Vec2f(su * si.du_dy, sv * si.dv_dy);

    return common::math::Vec2f(su * si.uv[0] + du, sv * si.uv[1] + dv);
}


}
}