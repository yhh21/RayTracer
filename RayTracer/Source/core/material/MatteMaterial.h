#pragma once

#include "../../ForwardDeclaration.h"
#include "Material.h"
#include "../color/Spectrum.h"

namespace core
{
namespace material
{


class MatteMaterial : public Material
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    MatteMaterial(const std::shared_ptr<core::texture::Texture<core::color::Spectrum>> &Kd,
        const std::shared_ptr<core::texture::Texture<Float>> &sigma,
        const std::shared_ptr<core::texture::Texture<Float>> &bumpMap)
        : Kd(Kd), sigma(sigma), bumpMap(bumpMap)
    {}


    void ComputeScatteringFunctions(core::interaction::SurfaceInteraction *si, common::tool::MemoryArena &arena,
        TransportMode mode,
        bool allowMultipleLobes) const;

private:

    std::shared_ptr<core::texture::Texture<core::color::Spectrum>> Kd;

    std::shared_ptr<core::texture::Texture<Float>> sigma, bumpMap;
};

/* TODO
MatteMaterial *CreateMatteMaterial(const TextureParams &mp);
*/


}
}