#pragma once

#include "Integrator.h"
#include "../camera/Camera.h"
#include "../sampler/Sampler.h"
#include "../scene/Scene.h"
#include "../../common/math/Bounds2.h"

namespace core
{
namespace integrator
{


class SamplerIntegrator : public Integrator
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    SamplerIntegrator(std::shared_ptr<const core::camera::Camera> camera,
        std::shared_ptr<core::sampler::Sampler> sampler,
        const common::math::Bounds2i &pixelBounds)
        : camera(camera), sampler(sampler), pixelBounds(pixelBounds)
    {}


    virtual void Preprocess(const core::scene::Scene &scene, core::sampler::Sampler &sampler)
    {}

    void Render(const core::scene::Scene &scene);

    virtual core::color::Spectrum Li(const common::math::RayDifferentialf &ray, const core::scene::Scene &scene,
        core::sampler::Sampler &sampler, common::tool::MemoryArena &arena,
        int depth = 0) const = 0;

    core::color::Spectrum SpecularReflect(const common::math::RayDifferentialf &ray,
        const core::interaction::SurfaceInteraction &isect,
        const core::scene::Scene &scene, core::sampler::Sampler &sampler,
        common::tool::MemoryArena &arena, int depth) const;

    core::color::Spectrum SpecularTransmit(const common::math::RayDifferentialf &ray,
        const core::interaction::SurfaceInteraction &isect,
        const core::scene::Scene &scene, core::sampler::Sampler &sampler,
        common::tool::MemoryArena &arena, int depth) const;

protected:

    std::shared_ptr<const core::camera::Camera> camera;

private:

    std::shared_ptr<core::sampler::Sampler> sampler;

    const common::math::Bounds2i pixelBounds;
};



}
}