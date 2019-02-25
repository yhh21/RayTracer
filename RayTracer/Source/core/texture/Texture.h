#pragma once

#include "../../ForwardDeclaration.h"

namespace core
{
namespace texture
{


template <typename T>
class Texture
{
public:
    virtual ~Texture()
    {}

    virtual T Evaluate(const interaction::SurfaceInteraction &si) const = 0;
};


Float Lanczos(Float, Float tau = FLOAT_0);

Float Noise(Float x, Float y = FLOAT_INV_2, Float z = FLOAT_INV_2);

Float Noise(const common::math::Vec3f &p);

Float FBm(const common::math::Vec3f &p, const common::math::Vec3f &dpdx, const common::math::Vec3f &dpdy,
    Float omega, int octaves);

Float Turbulence(const common::math::Vec3f &p, const common::math::Vec3f &dpdx, const common::math::Vec3f &dpdy,
    Float omega, int octaves);


}
}