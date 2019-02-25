#include "Light.h"
#include "../../common/math/RayDifferential.h"
#include "../color/Spectrum.h"

namespace core
{
namespace light
{


Light::Light(int flags, const common::math::Transformf &LightToWorld,
    const interaction::MediumInterface &medium_interface, int nSamples)
    : flags(flags),
    nSamples((std::max)(1, nSamples)),
    medium_interface(medium_interface),
    LightToWorld(LightToWorld),
    WorldToLight(Inverse(LightToWorld))
{}

Light::~Light()
{}


color::Spectrum Light::Le(const common::math::RayDifferentialf &ray) const
{
    return color::Spectrum(FLOAT_0);
}


}
}