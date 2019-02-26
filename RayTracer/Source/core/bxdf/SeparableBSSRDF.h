#pragma once

#include "BSSRDF.h"
#include "BxDF.h"
#include "Fresnel.h"
#include "../material/Material.h"
#include "../color/Spectrum.h"

namespace core
{
namespace bxdf
{


class SeparableBSSRDF : public BSSRDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    SeparableBSSRDF(const core::interaction::SurfaceInteraction &po, Float eta,
        const core::material::Material *material, core::material::TransportMode mode)
        : BSSRDF(po, eta),
        ns(po.shading.n),
        ss(Normalize(po.shading.dp_du)),
        ts(Cross(ns, ss)),
        material(material),
        mode(mode)
    {}


    core::color::Spectrum S(const core::interaction::SurfaceInteraction &pi, const common::math::Vec3f &wi)
    {
        //ProfilePhase pp(Prof::BSSRDFEvaluation);
        Float Ft = FrDielectric(CosTheta(po.wo), FLOAT_1, eta);
        return (FLOAT_1 - Ft) * Sp(pi) * Sw(wi);
    }

    core::color::Spectrum Sw(const common::math::Vec3f &w) const
    {
        Float c = FLOAT_1 - FLOAT_2 * FresnelMoment1(FLOAT_1 / eta);
        return (FLOAT_1 - FrDielectric(CosTheta(w), FLOAT_1, eta)) / (c * common::math::PI);
    }

    core::color::Spectrum Sp(const core::interaction::SurfaceInteraction &pi) const
    {
        return Sr(Distance(po.p, pi.p));
    }

    core::color::Spectrum Sample_S(const core::scene::Scene &scene, Float u1, const common::math::Vec2f &u2,
        common::tool::MemoryArena &arena, core::interaction::SurfaceInteraction *si,
        Float *pdf) const;

    core::color::Spectrum Sample_Sp(const core::scene::Scene &scene, Float u1, const common::math::Vec2f &u2,
        common::tool::MemoryArena &arena, core::interaction::SurfaceInteraction *si,
        Float *pdf) const;

    Float Pdf_Sp(const core::interaction::SurfaceInteraction &si) const;


    virtual core::color::Spectrum Sr(Float d) const = 0;

    virtual Float Sample_Sr(int ch, Float u) const = 0;

    virtual Float Pdf_Sr(int ch, Float r) const = 0;

private:

    const common::math::Vec3f ns;

    const common::math::Vec3f ss, ts;

    const core::material::Material *material;

    const core::material::TransportMode mode;


    friend class SeparableBSSRDFAdapter;
};


}
}