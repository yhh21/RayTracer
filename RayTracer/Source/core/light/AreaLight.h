#pragma once

#include "Light.h"

namespace core
{
namespace light
{


class AreaLight : public Light
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    AreaLight(const common::math::Transformf &LightToWorld, const core::interaction::MediumInterface &medium,
        int nSamples);


    virtual color::Spectrum L(const core::interaction::Interaction &intr, const common::math::Vec3f &w) const = 0;
};


}
}