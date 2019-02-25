#pragma once

#include "../../ForwardDeclaration.h"

namespace core
{
namespace texture
{


class TextureMapping2D
{
public:

    virtual ~TextureMapping2D();

    virtual common::math::Vec2f Map(const interaction::SurfaceInteraction &si, common::math::Vec2f *dstdx,
        common::math::Vec2f *dstdy) const = 0;
};


}
}