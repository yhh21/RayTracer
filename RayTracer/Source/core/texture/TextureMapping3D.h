#pragma once

#include "../../ForwardDeclaration.h"

namespace core
{
namespace texture
{


class TextureMapping3D
{
public:

    virtual ~TextureMapping3D();


    virtual common::math::Vec3f Map(const interaction::SurfaceInteraction &si, common::math::Vec3f *dpdx,
        common::math::Vec3f *dpdy) const = 0;
};


}
}