#include "PlanarMapping2D.h"
#include "../../common/math/Vec2.h"
#include "../interaction/SurfaceInteraction.h"

namespace core
{
namespace texture
{


common::math::Vec2f PlanarMapping2D::Map(const interaction::SurfaceInteraction &si, common::math::Vec2f *dstdx,
    common::math::Vec2f *dstdy) const
{
    common::math::Vec3f vec(si.p);
    *dstdx = common::math::Vec2f(Dot(si.dp_dx, vs), Dot(si.dp_dx, vt));
    *dstdy = common::math::Vec2f(Dot(si.dp_dy, vs), Dot(si.dp_dy, vt));

    return common::math::Vec2f(ds + Dot(vec, vs), dt + Dot(vec, vt));
}


}
}