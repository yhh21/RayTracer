#pragma once

#include "../../ForwardDeclaration.h"
#include "../../common/math/Constants.h"

namespace core
{
namespace interaction
{


inline
Float PhaseHG(Float cosTheta, Float g)
{
    Float denom = FLOAT_1 + g * g + FLOAT_2 * g * cosTheta;
    return common::math::INV_FOUR_PI * (FLOAT_1 - g * g) / (denom * std::sqrt(denom));
}

class PhaseFunction
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    virtual ~PhaseFunction();


    virtual Float p(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const = 0;

    virtual Float Sample_p(const common::math::Vec3f &wo, common::math::Vec3f *wi,
        const common::math::Vec2f &u) const = 0;
};


class Medium
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    virtual ~Medium()
    {}


    virtual core::color::Spectrum Tr(const common::math::Rayf &ray, core::sampler::Sampler &sampler) const = 0;

    virtual core::color::Spectrum Sample(const common::math::Rayf &ray, core::sampler::Sampler &sampler,
        common::tool::MemoryArena &arena,
        MediumInteraction *mi) const = 0;
};


struct MediumInterface
{
    const Medium *inside, *outside;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    MediumInterface() : inside(nullptr), outside(nullptr)
    {}

    MediumInterface(const Medium *medium) : inside(medium), outside(medium)
    {}

    MediumInterface(const Medium *inside, const Medium *outside)
        : inside(inside), outside(outside)
    {}


    bool IsMediumTransition() const
    {
        return inside != outside;
    }
};


}
}