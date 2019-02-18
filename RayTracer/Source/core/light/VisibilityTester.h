#pragma once

#include "../../ForwardDeclaration.h"
#include "../interaction/Interaction.h"

namespace core
{
namespace light
{


class VisibilityTester
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    VisibilityTester()
    {}

    VisibilityTester(const core::interaction::Interaction &p0, const core::interaction::Interaction &p1)
        : p0(p0), p1(p1)
    {}


    const core::interaction::Interaction &P0() const
    {
        return p0;
    }

    const core::interaction::Interaction &P1() const
    {
        return p1;
    }


    bool Unoccluded(const core::scene::Scene &scene) const;

    color::Spectrum Tr(const core::scene::Scene &scene, core::sampler::Sampler &sampler) const;

private:

    core::interaction::Interaction p0, p1;
};


}
}