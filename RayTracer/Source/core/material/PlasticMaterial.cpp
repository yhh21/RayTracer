#include "PlasticMaterial.h"
#include "../texture/Texture.h"
#include "../interaction/SurfaceInteraction.h"
#include "../bxdf/BSDF.h"
#include "../bxdf/LambertianReflection.h"
#include "../bxdf/MicrofacetReflection.h"
#include "../bxdf/distribution/TrowbridgeReitzDistribution.h"
#include "../../common/math/Constants.h"
#include "../../common/tool/MemoryArena.h"

namespace core
{
namespace material
{


void PlasticMaterial::ComputeScatteringFunctions(
    core::interaction::SurfaceInteraction *si, common::tool::MemoryArena &arena, TransportMode mode,
    bool allowMultipleLobes) const
{
    // Perform bump mapping with _bumpMap_, if present
    if (bumpMap)
    {
        Bump(bumpMap, si);
    }
    si->bsdf = ARENA_ALLOC(arena, core::bxdf::BSDF)(*si);
    // Initialize diffuse component of plastic material
    core::color::Spectrum kd = Kd->Evaluate(*si).Clamp();
    if (!kd.IsBlack())
    {
        si->bsdf->Add(ARENA_ALLOC(arena, core::bxdf::LambertianReflection)(kd));
    }

    // Initialize specular component of plastic material
    core::color::Spectrum ks = Ks->Evaluate(*si).Clamp();
    if (!ks.IsBlack())
    {
        core::bxdf::Fresnel *fresnel = ARENA_ALLOC(arena, core::bxdf::FresnelDielectric)(FLOAT_1 + FLOAT_INV_2, FLOAT_1);
        // Create microfacet distribution _distrib_ for plastic material
        Float rough = roughness->Evaluate(*si);
        if (remapRoughness)
        {
            rough = core::bxdf::distribution::TrowbridgeReitzDistribution::RoughnessToAlpha(rough);
        }
        core::bxdf::distribution::MicrofacetDistribution *distrib =
            ARENA_ALLOC(arena, core::bxdf::distribution::TrowbridgeReitzDistribution)(rough, rough);
        core::bxdf::BxDF *spec = ARENA_ALLOC(arena, core::bxdf::MicrofacetReflection)(ks, distrib, fresnel);
        si->bsdf->Add(spec);
    }
}

/* TODO
PlasticMaterial *CreatePlasticMaterial(const TextureParams &mp)
{
    std::shared_ptr<core::texture::Texture<core::color::Spectrum>> Kd =
        mp.GetSpectrumTexture("Kd", core::color::Spectrum(0.25f));
    std::shared_ptr<core::texture::Texture<core::color::Spectrum>> Ks =
        mp.GetSpectrumTexture("Ks", core::color::Spectrum(0.25f));
    std::shared_ptr<core::texture::Texture<Float>> roughness =
        mp.GetFloatTexture("roughness", .1f);
    std::shared_ptr<core::texture::Texture<Float>> bumpMap =
        mp.GetFloatTextureOrNull("bumpmap");
    bool remapRoughness = mp.FindBool("remaproughness", true);
    return new PlasticMaterial(Kd, Ks, roughness, bumpMap, remapRoughness);
}
*/

}
}