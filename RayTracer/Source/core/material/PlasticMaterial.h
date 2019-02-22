#pragma once

#include "../../ForwardDeclaration.h"
#include "Material.h"
#include "../color/Spectrum.h"

namespace core
{
namespace material
{


class PlasticMaterial : public Material
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    PlasticMaterial(const std::shared_ptr<core::texture::Texture<core::color::Spectrum>> &Kd,
        const std::shared_ptr<core::texture::Texture<core::color::Spectrum>> &Ks,
        const std::shared_ptr<core::texture::Texture<Float>> &roughness,
        const std::shared_ptr<core::texture::Texture<Float>> &bumpMap,
        bool remapRoughness)
        : Kd(Kd),
        Ks(Ks),
        roughness(roughness),
        bumpMap(bumpMap),
        remapRoughness(remapRoughness)
    {}


    void ComputeScatteringFunctions(core::interaction::SurfaceInteraction *si, common::tool::MemoryArena &arena,
        TransportMode mode,
        bool allowMultipleLobes) const;

private:

    std::shared_ptr<core::texture::Texture<core::color::Spectrum>> Kd, Ks;

    std::shared_ptr<core::texture::Texture<Float>> roughness, bumpMap;

    const bool remapRoughness;
};

/* TODO
PlasticMaterial *CreatePlasticMaterial(const TextureParams &mp);
*/


}
}