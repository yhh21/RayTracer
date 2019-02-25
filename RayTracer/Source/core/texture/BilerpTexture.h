#pragma once

#include "Texture.h"
#include "TextureMapping2D.h"
#include "../interaction/SurfaceInteraction.h"

namespace core
{
namespace texture
{


template <typename T>
class BilerpTexture : public Texture<T>
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    BilerpTexture(std::unique_ptr<TextureMapping2D> mapping, const T &v00,
        const T &v01, const T &v10, const T &v11)
        : mapping(std::move(mapping)), v00(v00), v01(v01), v10(v10), v11(v11)
    {}


    T Evaluate(const interaction::SurfaceInteraction &si) const
    {
        common::math::Vec2f dstdx, dstdy;
        common::math::Vec2f st = mapping->Map(si, &dstdx, &dstdy);

        return (static_cast<T>(1) - st[0]) * (static_cast<T>(1) - st[1]) * v00
            + (static_cast<T>(1) - st[0]) * (st[1]) * v01
            + (st[0]) * (static_cast<T>(1) - st[1]) * v10
            + (st[0]) * (st[1]) * v11;
    }

private:

    std::unique_ptr<TextureMapping2D> mapping;
    const T v00, v01, v10, v11;
};

/* TODO
BilerpTexture<Float> *CreateBilerpFloatTexture(const Transform &tex2world,
    const TextureParams &tp);

BilerpTexture<Spectrum> *CreateBilerpSpectrumTexture(const Transform &tex2world,
    const TextureParams &tp);
*/


}
}