#pragma once

#include "Texture.h"
#include "../interaction/SurfaceInteraction.h"

namespace core
{
namespace texture
{


template <typename T>
class MixTexture : public Texture<T>
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    MixTexture(const std::shared_ptr<Texture<T>> &tex1,
        const std::shared_ptr<Texture<T>> &tex2,
        const std::shared_ptr<Texture<Float>> &amount)
        : tex1(tex1), tex2(tex2), amount(amount)
    {}


    T Evaluate(const interaction::SurfaceInteraction &si) const
    {
        T t1 = tex1->Evaluate(si), t2 = tex2->Evaluate(si);
        Float amt = amount->Evaluate(si);
        return (static_cast<T>(1) - amt) * t1 + amt * t2;
    }

private:
    std::shared_ptr<Texture<T>> tex1, tex2;
    std::shared_ptr<Texture<Float>> amount;
};

/* TODO
MixTexture<Float> *CreateMixFloatTexture(const Transform &tex2world,
    const TextureParams &tp);

MixTexture<Spectrum> *CreateMixSpectrumTexture(const Transform &tex2world,
    const TextureParams &tp);
*/


}
}