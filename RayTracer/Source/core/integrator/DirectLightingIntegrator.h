#pragma once

#include "SamplerIntegrator.h"

namespace core
{
namespace integrator
{


enum class LightStrategy
{
    UniformSampleAll,
    UniformSampleOne
};


class DirectLightingIntegrator : public SamplerIntegrator
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    DirectLightingIntegrator(LightStrategy strategy, int maxDepth,
        std::shared_ptr<const core::camera::Camera> camera,
        std::shared_ptr<core::sampler::Sampler> sampler,
        const common::math::Bounds2i &pixelBounds)
        : SamplerIntegrator(camera, sampler, pixelBounds),
        strategy(strategy),
        maxDepth(maxDepth)
    {}


    core::color::Spectrum Li(const common::math::RayDifferentialf &ray, const core::scene::Scene &scene,
        core::sampler::Sampler &sampler, common::tool::MemoryArena &arena, int depth) const;

    void Preprocess(const core::scene::Scene &scene, core::sampler::Sampler &sampler);

private:

    const LightStrategy strategy;

    const int maxDepth;

    std::vector<int> nLightSamples;
};

/* TODO
DirectLightingIntegrator *CreateDirectLightingIntegrator(
    const ParamSet &params, std::shared_ptr<core::sampler::Sampler> sampler,
    std::shared_ptr<const core::camera::Camera> camera);
*/


}
}