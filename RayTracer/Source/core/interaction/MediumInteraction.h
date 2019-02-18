#pragma once

#include "../../ForwardDeclaration.h"
#include "Interaction.h"
#include "Medium.h"

namespace core
{
namespace interaction
{


class MediumInteraction : public Interaction
{
public:

    const PhaseFunction *phase;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    MediumInteraction() : phase(nullptr)
    {}

    MediumInteraction(const common::math::Vec3f &p, const common::math::Vec3f &wo, Float time,
        const Medium *medium, const PhaseFunction *phase)
        : Interaction(p, wo, time, medium), phase(phase)
    {}


    bool IsValid() const
    {
        return phase != nullptr;
    }
};


}
}