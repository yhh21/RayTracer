#include "FourierMaterial.h"
#include "../texture/Texture.h"
#include "../interaction/SurfaceInteraction.h"
#include "../bxdf/BSDF.h"
#include "../bxdf/FourierBSDF.h"
#include "../bxdf/LambertianReflection.h"
#include "../../common/math/Constants.h"
#include "../../common/tool/MemoryArena.h"

namespace core
{
namespace material
{

std::map<std::string, std::unique_ptr<core::bxdf::FourierBSDFTable>> FourierMaterial::loadedBSDFs;

FourierMaterial::FourierMaterial(const std::string &filename,
    const std::shared_ptr<core::texture::Texture<Float>> &bumpMap)
    : bumpMap(bumpMap)
{
    if (loadedBSDFs.find(filename) == loadedBSDFs.end())
    {
        std::unique_ptr<core::bxdf::FourierBSDFTable> table(new core::bxdf::FourierBSDFTable);
        /* TODO
        core::bxdf::FourierBSDFTable::Read(filename, table.get());
        */
        loadedBSDFs[filename] = std::move(table);
    }
    bsdfTable = loadedBSDFs[filename].get();
}

void FourierMaterial::ComputeScatteringFunctions(
    core::interaction::SurfaceInteraction *si, common::tool::MemoryArena &arena, TransportMode mode,
    bool allowMultipleLobes) const
{
    // Perform bump mapping with _bumpMap_, if present
    if (bumpMap)
    {
        Bump(bumpMap, si);
    }
    si->bsdf = ARENA_ALLOC(arena, core::bxdf::BSDF)(*si);
    // Checking for zero channels works as a proxy for checking whether the
    // table was successfully read from the file.
    if (bsdfTable->nChannels > 0)
    {
        si->bsdf->Add(ARENA_ALLOC(arena, core::bxdf::FourierBSDF)(*bsdfTable, mode));
    }
}

/* TODO
FourierMaterial *CreateFourierMaterial(const TextureParams &mp)
{
    std::shared_ptr<core::texture::Texture<Float>> bumpMap =
        mp.GetFloatTextureOrNull("bumpmap");
    return new FourierMaterial(mp.FindFilename("bsdffile"), bumpMap);
}
*/


}
}