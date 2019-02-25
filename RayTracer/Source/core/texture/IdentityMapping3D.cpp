#include "IdentityMapping3D.h"
#include "../interaction/SurfaceInteraction.h"

namespace core
{
namespace texture
{


common::math::Vec3f IdentityMapping3D::Map(const interaction::SurfaceInteraction &si, common::math::Vec3f *dpdx,
    common::math::Vec3f *dpdy) const
{
    *dpdx = WorldToTexture(si.dp_dx);
    *dpdy = WorldToTexture(si.dp_dy);

    return WorldToTexture(si.p);
}


}
}