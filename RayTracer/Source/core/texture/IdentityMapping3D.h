#pragma once

#include "TextureMapping3D.h"
#include "../../common/math/Vec3.h"
#include "../../common/math/Transform.h"

namespace core
{
namespace texture
{


class IdentityMapping3D : public TextureMapping3D
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    IdentityMapping3D(const common::math::Transformf &WorldToTexture)
        : WorldToTexture(WorldToTexture)
    {}


    common::math::Vec3f Map(const interaction::SurfaceInteraction &si, common::math::Vec3f *dpdx,
        common::math::Vec3f *dpdy) const;

private:

    const common::math::Transformf WorldToTexture;
};


}
}