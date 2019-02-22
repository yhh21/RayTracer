#pragma once

#include "../../ForwardDeclaration.h"
#include "Material.h"
#include "../color/Spectrum.h"

namespace core
{
namespace material
{


class MixMaterial : public Material
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    MixMaterial(const std::shared_ptr<Material> &m1,
        const std::shared_ptr<Material> &m2,
        const std::shared_ptr<core::texture::Texture<core::color::Spectrum>> &scale)
        : m1(m1), m2(m2), scale(scale)
    {}


    void ComputeScatteringFunctions(core::interaction::SurfaceInteraction *si, common::tool::MemoryArena &arena,
        TransportMode mode,
        bool allowMultipleLobes) const;

private:

    std::shared_ptr<Material> m1, m2;

    std::shared_ptr<core::texture::Texture<core::color::Spectrum>> scale;
};

/* TODO
MixMaterial *CreateMixMaterial(const TextureParams &mp,
    const std::shared_ptr<Material> &m1,
    const std::shared_ptr<Material> &m2);
*/


}
}