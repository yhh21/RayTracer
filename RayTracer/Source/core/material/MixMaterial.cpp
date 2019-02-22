#include "MixMaterial.h"
#include "../texture/Texture.h"
#include "../interaction/SurfaceInteraction.h"
#include "../bxdf/BSDF.h"
#include "../bxdf/ScaledBxDF.h"
#include "../../common/math/Constants.h"
#include "../../common/tool/MemoryArena.h"

namespace core
{
namespace material
{


void MixMaterial::ComputeScatteringFunctions(core::interaction::SurfaceInteraction *si,
    common::tool::MemoryArena &arena,
    TransportMode mode,
    bool allowMultipleLobes) const
{
    // Compute weights and original _BxDF_s for mix material
    core::color::Spectrum s1 = scale->Evaluate(*si).Clamp();
    core::color::Spectrum s2 = (core::color::Spectrum(FLOAT_1) - s1).Clamp();
    core::interaction::SurfaceInteraction si2 = *si;
    m1->ComputeScatteringFunctions(si, arena, mode, allowMultipleLobes);
    m2->ComputeScatteringFunctions(&si2, arena, mode, allowMultipleLobes);

    // Initialize _si->bsdf_ with weighted mixture of _BxDF_s
    int n1 = si->bsdf->NumComponents(), n2 = si2.bsdf->NumComponents();
    for (int i = 0; i < n1; ++i)
    {
        si->bsdf->bxdfs[i] = ARENA_ALLOC(arena, core::bxdf::ScaledBxDF)(si->bsdf->bxdfs[i], s1);
    }
    for (int i = 0; i < n2; ++i)
    {
        si->bsdf->Add(ARENA_ALLOC(arena, core::bxdf::ScaledBxDF)(si2.bsdf->bxdfs[i], s2));
    }
}

/* TODO
MixMaterial *CreateMixMaterial(const TextureParams &mp,
    const std::shared_ptr<Material> &m1,
    const std::shared_ptr<Material> &m2)
{
    std::shared_ptr<core::texture::Texture<core::color::Spectrum>> scale =
        mp.GetSpectrumTexture("amount", core::color::Spectrum(0.5f));
    return new MixMaterial(m1, m2, scale);
}
*/


}
}