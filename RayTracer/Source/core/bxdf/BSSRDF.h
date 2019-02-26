#pragma once

#include "../../ForwardDeclaration.h"
#include "../interaction/SurfaceInteraction.h"

namespace core
{
namespace bxdf
{

Float FresnelMoment1(Float invEta);

Float FresnelMoment2(Float invEta);


Float CatmullRom(int size, const Float *nodes, const Float *values, Float x);

bool CatmullRomWeights(int size, const Float *nodes, Float x, int *offset,
    Float *weights);

Float SampleCatmullRom(int size, const Float *nodes, const Float *f,
    const Float *cdf, Float sample, Float *fval = nullptr,
    Float *pdf = nullptr);

Float SampleCatmullRom2D(int size1, int size2, const Float *nodes1,
    const Float *nodes2, const Float *values,
    const Float *cdf, Float alpha, Float sample,
    Float *fval = nullptr, Float *pdf = nullptr);

Float IntegrateCatmullRom(int n, const Float *nodes, const Float *values,
    Float *cdf);

Float InvertCatmullRom(int n, const Float *x, const Float *values, Float u);


class BSSRDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    BSSRDF(const core::interaction::SurfaceInteraction &po, Float eta) : po(po), eta(eta)
    {}

    virtual ~BSSRDF()
    {}


    virtual core::color::Spectrum S(const core::interaction::SurfaceInteraction &pi
        , const common::math::Vec3f &wi) = 0;

    virtual core::color::Spectrum Sample_S(const core::scene::Scene &scene
        , Float u1, const common::math::Vec2f &u2,
        common::tool::MemoryArena &arena, core::interaction::SurfaceInteraction *si,
        Float *pdf) const = 0;

protected:

    const core::interaction::SurfaceInteraction &po;

    Float eta;
};


}
}