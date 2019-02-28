#pragma once

#include "../../ForwardDeclaration.h"
#include <vector>

namespace core
{
namespace integrator
{


class Integrator
{
public:

    virtual ~Integrator();


    virtual void Render(const core::scene::Scene &scene) = 0;
};


core::color::Spectrum UniformSampleAllLights(const core::interaction::Interaction &it,
    const core::scene::Scene &scene,
    common::tool::MemoryArena &arena, core::sampler::Sampler &sampler,
    const std::vector<int> &nLightSamples,
    bool handleMedia = false);

core::color::Spectrum UniformSampleOneLight(const core::interaction::Interaction &it, const core::scene::Scene &scene,
    common::tool::MemoryArena &arena, core::sampler::Sampler &sampler,
    bool handleMedia = false,
    const core::sampler::Distribution1D *lightDistrib = nullptr);

core::color::Spectrum EstimateDirect(const core::interaction::Interaction &it, const common::math::Vec2f &uShading,
    const core::light::Light &light, const common::math::Vec2f &uLight,
    const core::scene::Scene &scene, core::sampler::Sampler &sampler,
    common::tool::MemoryArena &arena, bool handleMedia = false,
    bool specular = false);

std::unique_ptr<core::sampler::Distribution1D> ComputeLightPowerDistribution(
    const core::scene::Scene &scene);

}
}