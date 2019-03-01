#include "DirectLightingIntegrator.h"
#include "../bxdf/BxDF.h"
#include "../bxdf/BSDF.h"
#include "../color/Spectrum.h"
#include "../interaction/Interaction.h"
#include "../interaction/SurfaceInteraction.h"
#include "../interaction/MediumInteraction.h"
#include "../light/Light.h"
#include "../light/AreaLight.h"
#include "../light/VisibilityTester.h"
#include "../primitive/Primitive.h"
#include "../scene/Scene.h"
#include "../sampler/Sampler.h"
#include "../sampler/Sampling.h"
#include "../../common/math/RayDifferential.h"
#include "../../common/tool/MemoryArena.h"

namespace core
{
namespace integrator
{


void DirectLightingIntegrator::Preprocess(const core::scene::Scene &scene,
    core::sampler::Sampler &sampler)
{
    if (LightStrategy::UniformSampleAll == strategy)
    {
        // Compute number of samples to use for each light
        for (const auto &light : scene.lights)
        {
            nLightSamples.push_back(sampler.RoundCount(light->nSamples));
        }

        // Request samples for sampling all lights
        for (int i = 0; i < maxDepth; ++i)
        {
            for (size_t j = 0; j < scene.lights.size(); ++j)
            {
                sampler.Request2DArray(nLightSamples[j]);
                sampler.Request2DArray(nLightSamples[j]);
            }
        }
    }
}

core::color::Spectrum DirectLightingIntegrator::Li(const common::math::RayDifferentialf &ray,
    const core::scene::Scene &scene, core::sampler::Sampler &sampler,
    common::tool::MemoryArena &arena, int depth) const
{
    //ProfilePhase p(Prof::SamplerIntegratorLi);
    core::color::Spectrum L(FLOAT_0);
    // Find closest ray intersection or return background radiance
    core::interaction::SurfaceInteraction isect;
    if (!scene.Intersect(ray, &isect))
    {
        for (const auto &light : scene.lights)
        {
            L += light->Le(ray);
        }
        return L;
    }

    // Compute scattering functions for surface interaction
    isect.ComputeScatteringFunctions(ray, arena);
    if (!isect.bsdf)
    {
        return Li(isect.SpawnRay(ray.dir), scene, sampler, arena, depth);
    }
    common::math::Vec3f wo = isect.wo;
    // Compute emitted light if ray hit an area light source
    L += isect.Le(wo);
    if (scene.lights.size() > 0)
    {
        // Compute direct lighting for _DirectLightingIntegrator_ integrator
        if (LightStrategy::UniformSampleAll == strategy)
        {
            L += UniformSampleAllLights(isect, scene, arena, sampler,
                nLightSamples);
        }
        else
        {
            L += UniformSampleOneLight(isect, scene, arena, sampler);
        }
    }
    if (depth + 1 < maxDepth)
    {
        // Trace rays for specular reflection and refraction
        L += SpecularReflect(ray, isect, scene, sampler, arena, depth);
        L += SpecularTransmit(ray, isect, scene, sampler, arena, depth);
    }
    return L;
}

/* TODO
DirectLightingIntegrator *CreateDirectLightingIntegrator(
    const ParamSet &params, std::shared_ptr<core::sampler::Sampler> sampler,
    std::shared_ptr<const core::camera::Camera> camera)
{
    int maxDepth = params.FindOneInt("maxdepth", 5);
    LightStrategy strategy;
    std::string st = params.FindOneString("strategy", "all");
    if ("one" == st)
        strategy = LightStrategy::UniformSampleOne;
    else if ("all" == st)
        strategy = LightStrategy::UniformSampleAll;
    else
    {
        Warning(
            "Strategy \"%s\" for direct lighting unknown. "
            "Using \"all\".",
            st.c_str());
        strategy = LightStrategy::UniformSampleAll;
    }
    int np;
    const int *pb = params.FindInt("pixelbounds", &np);
    common::math::Bounds2i pixelBounds = camera->film->GetSampleBounds();
    if (pb)
    {
        if (np != 4)
            Error("Expected four values for \"pixelbounds\" parameter. Got %d.",
                np);
        else
        {
            pixelBounds = Intersect(pixelBounds,
                common::math::Bounds2i{{pb[0], pb[2]}, {pb[1], pb[3]}});
            if (0 == pixelBounds.Area())
                Error("Degenerate \"pixelbounds\" specified.");
        }
    }
    return new DirectLightingIntegrator(strategy, maxDepth, camera, sampler,
        pixelBounds);
}
*/


}
}