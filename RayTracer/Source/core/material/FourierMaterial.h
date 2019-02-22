#pragma once

#include "../../ForwardDeclaration.h"
#include "Material.h"
#include "../color/Spectrum.h"
#include <map>

namespace core
{
namespace material
{


class FourierMaterial : public Material
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    FourierMaterial(const std::string &filename,
        const std::shared_ptr<core::texture::Texture<Float>> &bump);


    void ComputeScatteringFunctions(core::interaction::SurfaceInteraction *si, common::tool::MemoryArena &arena,
        TransportMode mode,
        bool allowMultipleLobes) const;

private:

    core::bxdf::FourierBSDFTable *bsdfTable;

    std::shared_ptr<core::texture::Texture<Float>> bumpMap;

    static std::map<std::string, std::unique_ptr<core::bxdf::FourierBSDFTable>> loadedBSDFs;
};

/* TODO
FourierMaterial *CreateFourierMaterial(const TextureParams &mp);
*/


}
}