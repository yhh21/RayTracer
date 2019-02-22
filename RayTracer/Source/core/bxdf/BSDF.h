#pragma once

#include "BxDF.h"
#include "../interaction/SurfaceInteraction.h"

namespace core
{
namespace bxdf
{


class BSDF
{
public:

    const Float eta;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    BSDF(const interaction::SurfaceInteraction &si, Float eta = FLOAT_1)
        : eta(eta),
        ns(si.shading.n),
        ng(si.n),
        ss(Normalize(si.shading.dp_du)),
        ts(Cross(ns, ss))
    {}


    void Add(BxDF *b)
    {
        CHECK_LT(nBxDFs, MaxBxDFs);
        bxdfs[nBxDFs++] = b;
    }

    int NumComponents(BxDFType flags = BSDF_ALL) const;

    common::math::Vec3f WorldToLocal(const common::math::Vec3f &v) const
    {
        return common::math::Vec3f(Dot(v, ss), Dot(v, ts), Dot(v, ns));
    }

    common::math::Vec3f LocalToWorld(const common::math::Vec3f &v) const
    {
        return common::math::Vec3f(ss.x * v.x + ts.x * v.y + ns.x * v.z,
            ss.y * v.x + ts.y * v.y + ns.y * v.z,
            ss.z * v.x + ts.z * v.y + ns.z * v.z);
    }


    color::Spectrum f(const common::math::Vec3f &woW, const common::math::Vec3f &wiW,
        BxDFType flags = BSDF_ALL) const;

    color::Spectrum rho(int nSamples, const common::math::Vec2f *samples1, const common::math::Vec2f *samples2,
        BxDFType flags = BSDF_ALL) const;

    color::Spectrum rho(const common::math::Vec3f &wo, int nSamples, const common::math::Vec2f *samples,
        BxDFType flags = BSDF_ALL) const;

    color::Spectrum Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi, const common::math::Vec2f &u,
        Float *pdf, BxDFType type = BSDF_ALL,
        BxDFType *sampledType = nullptr) const;

    Float Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi,
        BxDFType flags = BSDF_ALL) const;

private:

    ~BSDF()
    {}


    const common::math::Vec3f ns, ng;
    const common::math::Vec3f ss, ts;
    int nBxDFs = 0;
    static constexpr int MaxBxDFs = 8;
    BxDF *bxdfs[MaxBxDFs];
    friend class core::material::MixMaterial;
};


inline
int BSDF::NumComponents(BxDFType flags) const
{
    int num = 0;
    for (int i = 0; i < nBxDFs; ++i)
    {
        if (bxdfs[i]->MatchesFlags(flags))
        {
            ++num;
        }
    }

    return num;
}

}
}