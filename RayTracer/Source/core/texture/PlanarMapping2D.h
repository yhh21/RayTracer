#pragma once

#include "TextureMapping2D.h"
#include "../../common/math/Vec3.h"
#include "../../common/math/Transform.h"

namespace core
{
namespace texture
{


class PlanarMapping2D : public TextureMapping2D
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    PlanarMapping2D(const common::math::Vec3f &vs, const common::math::Vec3f &vt, Float ds = FLOAT_0,
        Float dt = FLOAT_0)
        : vs(vs), vt(vt), ds(ds), dt(dt)
    {}


    common::math::Vec2f Map(const interaction::SurfaceInteraction &si, common::math::Vec2f *dstdx,
        common::math::Vec2f *dstdy) const;

private:

    const common::math::Vec3f vs, vt;
    const Float ds, dt;
};


}
}