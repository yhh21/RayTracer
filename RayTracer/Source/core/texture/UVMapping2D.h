#pragma once

#include "TextureMapping2D.h"

namespace core
{
namespace texture
{


class UVMapping2D : public TextureMapping2D
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    UVMapping2D(Float su = FLOAT_1, Float sv = FLOAT_1, Float du = FLOAT_0, Float dv = FLOAT_0);


    virtual common::math::Vec2f Map(const interaction::SurfaceInteraction &si, common::math::Vec2f *dstdx,
        common::math::Vec2f *dstdy) const = 0;

private:
    const Float su, sv, du, dv;
};


}
}