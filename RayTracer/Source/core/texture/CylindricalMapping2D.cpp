#include "CylindricalMapping2D.h"
#include "../interaction/SurfaceInteraction.h"

namespace core
{
namespace texture
{


common::math::Vec2f CylindricalMapping2D::Map(const interaction::SurfaceInteraction &si, common::math::Vec2f *dstdx,
    common::math::Vec2f *dstdy) const
{
    common::math::Vec2f st = cylinder(si.p);
    // Compute texture coordinate differentials for cylinder $(u,v)$ mapping
    const Float delta = static_cast<Float>(0.01F);
    common::math::Vec2f stDeltaX = cylinder(si.p + delta * si.dp_dx);
    *dstdx = (stDeltaX - st) / delta;
    if ((*dstdx)[1] > FLOAT_INV_2)
    {
        (*dstdx)[1] = FLOAT_1 - (*dstdx)[1];
    }
    else if ((*dstdx)[1] < -FLOAT_INV_2)
    {
        (*dstdx)[1] = -((*dstdx)[1] + FLOAT_1);
    }

    common::math::Vec2f stDeltaY = cylinder(si.p + delta * si.dp_dy);
    *dstdy = (stDeltaY - st) / delta;
    if ((*dstdy)[1] > FLOAT_INV_2)
    {
        (*dstdy)[1] = FLOAT_1 - (*dstdy)[1];
    }
    else if ((*dstdy)[1] < -FLOAT_INV_2)
    {
        (*dstdy)[1] = -((*dstdy)[1] + FLOAT_1);
    }

    return st;
}


}
}