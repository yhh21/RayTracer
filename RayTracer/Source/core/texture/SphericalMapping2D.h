#pragma once

#include "TextureMapping2D.h"
#include "../../common/math/Transform.h"

namespace core
{
namespace texture
{


class SphericalMapping2D : public TextureMapping2D
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    SphericalMapping2D(const common::math::Transformf &WorldToTexture)
        : WorldToTexture(WorldToTexture)
    {}


    common::math::Vec2f Map(const interaction::SurfaceInteraction &si, common::math::Vec2f *dstdx,
        common::math::Vec2f *dstdy) const;

private:
    common::math::Vec2f sphere(const common::math::Vec3f &P) const;

    const common::math::Transformf WorldToTexture;
};


}
}