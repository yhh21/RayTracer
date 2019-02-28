#pragma once

#include "SamplerIntegrator.h"

namespace core
{
namespace integrator
{


class PathIntegrator : public SamplerIntegrator
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    PathIntegrator(int maxDepth, std::shared_ptr<const core::camera::Camera> camera,
        std::shared_ptr<core::sampler::Sampler> sampler,
        const common::math::Bounds2i &pixelBounds, Float rrThreshold = FLOAT_1,
        const std::string &lightSampleStrategy = "spatial");


    void Preprocess(const core::scene::Scene &scene, core::sampler::Sampler &sampler);

    core::color::Spectrum Li(const common::math::RayDifferentialf &ray, const core::scene::Scene &scene,
        core::sampler::Sampler &sampler, common::tool::MemoryArena &arena, int depth) const;

private:

    const int maxDepth;

    const Float rrThreshold;

    const std::string lightSampleStrategy;

    std::unique_ptr<LightDistribution> lightDistribution;
};

/* TODO
PathIntegrator *CreatePathIntegrator(const ParamSet &params,
    std::shared_ptr<core::sampler::Sampler> sampler,
    std::shared_ptr<const core::camera::Camera> camera);
*/


}
}