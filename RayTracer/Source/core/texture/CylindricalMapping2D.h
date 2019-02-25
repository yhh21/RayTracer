#pragma once

#include "TextureMapping2D.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Vec3.h"
#include "../../common/math/Transform.h"

namespace core
{
namespace texture
{


class CylindricalMapping2D : public TextureMapping2D
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    CylindricalMapping2D(const common::math::Transformf &WorldToTexture)
        : WorldToTexture(WorldToTexture)
    {}


    common::math::Vec2f Map(const interaction::SurfaceInteraction &si, common::math::Vec2f *dstdx,
        common::math::Vec2f *dstdy) const;

private:

    common::math::Vec2f cylinder(const common::math::Vec3f &p) const
    {
        common::math::Vec3f vec = Normalize(WorldToTexture(p) - common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0));
        return common::math::Vec2f((common::math::PI + std::atan2(vec.y, vec.x)) * common::math::INV_TWO_PI, vec.z);
    }

    const common::math::Transformf WorldToTexture;
};


}
}