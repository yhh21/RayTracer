#include "BSDF.h"

namespace core
{
namespace bxdf
{


color::Spectrum BSDF::f(const common::math::Vec3f &woW, const common::math::Vec3f &wiW,
    BxDFType flags) const
{
    //ProfilePhase pp(Prof::BSDFEvaluation);
    common::math::Vec3f wi = WorldToLocal(wiW), wo = WorldToLocal(woW);
    if (FLOAT_0 == wo.z)
    {
        return FLOAT_0;
    }
    bool reflect = Dot(wiW, ng) * Dot(woW, ng) > FLOAT_0;
    color::Spectrum f(FLOAT_0);
    for (int i = 0; i < nBxDFs; ++i)
    {
        if (bxdfs[i]->MatchesFlags(flags) &&
            ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
            (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
        {
            f += bxdfs[i]->f(wo, wi);
        }
    }
    return f;
}

color::Spectrum BSDF::rho(int nSamples, const common::math::Vec2f *samples1,
    const common::math::Vec2f *samples2, BxDFType flags) const
{
    color::Spectrum ret(0.f);
    for (int i = 0; i < nBxDFs; ++i)
    {
        if (bxdfs[i]->MatchesFlags(flags))
        {
            ret += bxdfs[i]->rho(nSamples, samples1, samples2);
        }
    }

    return ret;
}

color::Spectrum BSDF::rho(const common::math::Vec3f &wo, int nSamples, const common::math::Vec2f *samples,
    BxDFType flags) const
{
    color::Spectrum ret(FLOAT_0);
    for (int i = 0; i < nBxDFs; ++i)
    {
        if (bxdfs[i]->MatchesFlags(flags))
        {
            ret += bxdfs[i]->rho(wo, nSamples, samples);
        }
    }

    return ret;
}

color::Spectrum BSDF::Sample_f(const common::math::Vec3f &woWorld, common::math::Vec3f *wiWorld,
    const common::math::Vec2f &u, Float *pdf, BxDFType type,
    BxDFType *sampledType) const
{
    //ProfilePhase pp(Prof::BSDFSampling);
    // Choose which _BxDF_ to sample
    int matchingComps = NumComponents(type);
    if (0 == matchingComps)
    {
        *pdf = FLOAT_0;
        if (sampledType) *sampledType = BxDFType(0);
        return color::Spectrum(FLOAT_0);
    }
    int comp = (std::min)((int)std::floor(u[0] * matchingComps), matchingComps - 1);

    // Get _BxDF_ pointer for chosen component
    BxDF *_bxdf = nullptr;
    int count = comp;
    for (int i = 0; i < nBxDFs; ++i)
    {
        if (bxdfs[i]->MatchesFlags(type) && 0 == count--)
        {
            _bxdf = bxdfs[i];
            break;
        }
    }
    CHECK(nullptr != _bxdf);
    /*
    VLOG(2) << "BSDF::Sample_f chose comp = " << comp << " / matching = " <<
        matchingComps << ", _bxdf: " << _bxdf->ToString();
    */

    // Remap _BxDF_ sample _u_ to $[0,1)^2$
    common::math::Vec2f uRemapped((std::min)(u[0] * matchingComps - comp, common::math::ONE_MINUS_MACHINE_EPSILON), u[1]);

    // Sample chosen _BxDF_
    common::math::Vec3f wi, wo = WorldToLocal(woWorld);
    if (FLOAT_0 == wo.z)
    {
        return color::Spectrum(FLOAT_0);;
    }
    *pdf = FLOAT_0;
    if (sampledType)
    {
        *sampledType = _bxdf->type;
    }
    color::Spectrum f = _bxdf->Sample_f(wo, &wi, uRemapped, pdf, sampledType);
    /*
    VLOG(2) << "For wo = " << wo << ", sampled f = " << f << ", pdf = "
        << *pdf << ", ratio = " << ((*pdf > 0) ? (f / *pdf) : color::Spectrum(0.))
        << ", wi = " << wi;
    */
    if (FLOAT_0 == *pdf)
    {
        if (nullptr != sampledType)
        {
            *sampledType = BxDFType(0);
        }
        return color::Spectrum(FLOAT_0);;
    }
    *wiWorld = LocalToWorld(wi);

    // Compute overall PDF with all matching _BxDF_s
    if (!(_bxdf->type & BSDF_SPECULAR) && matchingComps > 1)
    {
        for (int i = 0; i < nBxDFs; ++i)
        {
            if (bxdfs[i] != _bxdf && bxdfs[i]->MatchesFlags(type))
            {
                *pdf += bxdfs[i]->Pdf(wo, wi);
            }
        }
    }
    if (matchingComps > 1)
    {
        *pdf /= matchingComps;
    }

    // Compute value of BSDF for sampled direction
    if (!(_bxdf->type & BSDF_SPECULAR))
    {
        bool reflect = Dot(*wiWorld, ng) * Dot(woWorld, ng) > 0;
        f = FLOAT_0;
        for (int i = 0; i < nBxDFs; ++i)
        {
            if (bxdfs[i]->MatchesFlags(type) &&
                ((reflect && (bxdfs[i]->type & BSDF_REFLECTION)) ||
                (!reflect && (bxdfs[i]->type & BSDF_TRANSMISSION))))
            {
                f += bxdfs[i]->f(wo, wi);
            }
        }
    }
    /*
    VLOG(2) << "Overall f = " << f << ", pdf = " << *pdf << ", ratio = "
        << ((*pdf > 0) ? (f / *pdf) : color::Spectrum(0.));
    */
    return f;
}

Float BSDF::Pdf(const common::math::Vec3f &woWorld, const common::math::Vec3f &wiWorld,
    BxDFType flags) const
{
    //ProfilePhase pp(Prof::BSDFPdf);
    if (FLOAT_0 == nBxDFs)
    {
        return FLOAT_0;
    }
    common::math::Vec3f wo = WorldToLocal(woWorld), wi = WorldToLocal(wiWorld);
    if (FLOAT_0 == wo.z)
    {
        return FLOAT_0;
    }
    Float pdf = FLOAT_0;
    int matchingComps = 0;
    for (int i = 0; i < nBxDFs; ++i)
    {
        if (bxdfs[i]->MatchesFlags(flags))
        {
            ++matchingComps;
            pdf += bxdfs[i]->Pdf(wo, wi);
        }
    }
    Float v = matchingComps > 0 ? pdf / matchingComps : FLOAT_0;
    return v;
}


}
}