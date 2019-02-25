#pragma once

#include "Texture.h"
#include "../interaction/SurfaceInteraction.h"

namespace core
{
namespace texture
{


template <typename T>
class ConstantTexture : public Texture<T>
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    ConstantTexture(const T &value) : value(value)
    {}


    T Evaluate(const interaction::SurfaceInteraction &) const
    {
        return value;
    }

private:

    T value;
};

/* TODO
ConstantTexture<Float> *CreateConstantFloatTexture(const common::math::Transformf &tex2world,
    const TextureParams &tp);

ConstantTexture<Spectrum> *CreateConstantSpectrumTexture(
    const common::math::Transformf &tex2world, const TextureParams &tp);
*/


}
}