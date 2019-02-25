#include "SphericalMapping2D.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Vec3.h"
#include "../interaction/SurfaceInteraction.h"

namespace core
{
namespace texture
{


common::math::Vec2f SphericalMapping2D::Map(const interaction::SurfaceInteraction &si, common::math::Vec2f *dstdx,
    common::math::Vec2f *dstdy) const
{
    common::math::Vec2f st = sphere(si.p);
    // Compute texture coordinate differentials for sphere $(u,v)$ mapping
    const Float delta = static_cast<Float>(0.1F);
    common::math::Vec2f stDeltaX = sphere(si.p + delta * si.dp_dx);
    *dstdx = (stDeltaX - st) / delta;
    common::math::Vec2f stDeltaY = sphere(si.p + delta * si.dp_dy);
    *dstdy = (stDeltaY - st) / delta;

    // Handle sphere mapping discontinuity for coordinate differentials
    if ((*dstdx)[1] > FLOAT_INV_2)
    {
        (*dstdx)[1] = FLOAT_1 - (*dstdx)[1];
    }
    else if ((*dstdx)[1] < -FLOAT_INV_2)
    {
        (*dstdx)[1] = -((*dstdx)[1] + FLOAT_1);
    }
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

common::math::Vec2f SphericalMapping2D::sphere(const common::math::Vec3f &p) const
{
    common::math::Vec3f vec = Normalize(WorldToTexture(p) - common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0));
    Float theta = SphericalTheta(vec), phi = SphericalPhi(vec);
    return common::math::Vec2f(theta * common::math::INV_PI, phi * common::math::INV_TWO_PI);
}


}
}