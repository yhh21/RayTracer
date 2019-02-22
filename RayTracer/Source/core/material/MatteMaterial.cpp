#include "MatteMaterial.h"
#include "../texture/Texture.h"
#include "../interaction/SurfaceInteraction.h"
#include "../bxdf/BSDF.h"
#include "../bxdf/LambertianReflection.h"
#include "../bxdf/OrenNayar.h"
#include "../../common/math/Constants.h"
#include "../../common/tool/MemoryArena.h"

namespace core
{
namespace material
{


void MatteMaterial::ComputeScatteringFunctions(core::interaction::SurfaceInteraction *si,
    common::tool::MemoryArena &arena,
    TransportMode mode,
    bool allowMultipleLobes) const
{
    // Perform bump mapping with _bumpMap_, if present
    if (bumpMap)
    {
        Bump(bumpMap, si);
    }

    // Evaluate core::texture::Textures for _MatteMaterial_ material and allocate BRDF
    si->bsdf = ARENA_ALLOC(arena, core::bxdf::BSDF)(*si);
    core::color::Spectrum r = Kd->Evaluate(*si).Clamp();
    Float sig = common::math::Clamp(sigma->Evaluate(*si), FLOAT_0, static_cast<Float>(90.0F));
    if (!r.IsBlack())
    {
        if (FLOAT_0 == sig)
        {
            si->bsdf->Add(ARENA_ALLOC(arena, core::bxdf::LambertianReflection)(r));
        }
        else
        {
            si->bsdf->Add(ARENA_ALLOC(arena, core::bxdf::OrenNayar)(r, sig));
        }
    }
}

/* TODO
MatteMaterial *CreateMatteMaterial(const core::texture::TextureParams &mp)
{
    std::shared_ptr<core::texture::Texture<core::color::Spectrum>> Kd =
        mp.Getcore::color::Spectrumcore::texture::Texture("Kd", core::color::Spectrum(0.5f));
    std::shared_ptr<core::texture::Texture<Float>> sigma = mp.GetFloatcore::texture::Texture("sigma", 0.f);
    std::shared_ptr<core::texture::Texture<Float>> bumpMap =
        mp.GetFloatcore::texture::TextureOrNull("bumpmap");
    return new MatteMaterial(Kd, sigma, bumpMap);
}
*/

}
}