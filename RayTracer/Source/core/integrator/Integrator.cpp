#include "Integrator.h"
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


Integrator::~Integrator()
{

}


core::color::Spectrum UniformSampleAllLights(const core::interaction::Interaction &it, const core::scene::Scene &scene,
    common::tool::MemoryArena &arena, core::sampler::Sampler &sampler,
    const std::vector<int> &nLightSamples,
    bool handleMedia)
{
    //ProfilePhase p(Prof::DirectLighting);
    core::color::Spectrum L(FLOAT_0);
    for (size_t j = 0; j < scene.lights.size(); ++j)
    {
        // Accumulate contribution of _j_th light to _L_
        const std::shared_ptr<core::light::Light> &light = scene.lights[j];
        int nSamples = nLightSamples[j];
        const common::math::Vec2f *uLightArray = sampler.Get2DArray(nSamples);
        const common::math::Vec2f *uScatteringArray = sampler.Get2DArray(nSamples);
        if (!uLightArray || !uScatteringArray)
        {
            // Use a single sample for illumination from _light_
            common::math::Vec2f uLight = sampler.Get2D();
            common::math::Vec2f uScattering = sampler.Get2D();
            L += EstimateDirect(it, uScattering, *light, uLight, scene, sampler,
                arena, handleMedia);
        }
        else
        {
            // Estimate direct lighting using sample arrays
            core::color::Spectrum Ld(FLOAT_0);
            for (int k = 0; k < nSamples; ++k)
            {
                Ld += EstimateDirect(it, uScatteringArray[k], *light,
                    uLightArray[k], scene, sampler, arena,
                    handleMedia);
            }
            L += Ld / static_cast<Float>(nSamples);
        }
    }
    return L;
}

core::color::Spectrum UniformSampleOneLight(const core::interaction::Interaction &it, const core::scene::Scene &scene,
    common::tool::MemoryArena &arena, core::sampler::Sampler &sampler,
    bool handleMedia, const core::sampler::Distribution1D *lightDistrib)
{
    //ProfilePhase p(Prof::DirectLighting);
    // Randomly choose a single light to sample, _light_
    int nLights = static_cast<int>(scene.lights.size());
    if (0 == nLights)
    {
        return core::color::Spectrum(FLOAT_0);
    }
    int lightNum;
    Float lightPdf;
    if (lightDistrib)
    {
        lightNum = lightDistrib->SampleDiscrete(sampler.Get1D(), &lightPdf);
        if (0 == lightPdf)
        {
            return core::color::Spectrum(FLOAT_0);
        }
    }
    else
    {
        lightNum = (std::min)(static_cast<int>(sampler.Get1D() * nLights), nLights - 1);
        lightPdf = FLOAT_1 / nLights;
    }
    const std::shared_ptr<core::light::Light> &light = scene.lights[lightNum];
    common::math::Vec2f uLight = sampler.Get2D();
    common::math::Vec2f uScattering = sampler.Get2D();
    return EstimateDirect(it, uScattering, *light, uLight,
        scene, sampler, arena, handleMedia) / lightPdf;
}

core::color::Spectrum EstimateDirect(const core::interaction::Interaction &it, const common::math::Vec2f &uScattering,
    const core::light::Light &light, const common::math::Vec2f &uLight,
    const core::scene::Scene &scene, core::sampler::Sampler &sampler,
    common::tool::MemoryArena &arena, bool handleMedia, bool specular)
{
    core::bxdf::BxDFType bsdfFlags = specular ? core::bxdf::BxDFType::BSDF_ALL
        : core::bxdf::BxDFType(core::bxdf::BxDFType::BSDF_ALL & ~core::bxdf::BxDFType::BSDF_SPECULAR);
    core::color::Spectrum Ld(FLOAT_0);
    // Sample light source with multiple importance sampling
    common::math::Vec3f wi;
    Float lightPdf = FLOAT_0, scatteringPdf = FLOAT_0;
    core::light::VisibilityTester visibility;
    core::color::Spectrum Li = light.Sample_Li(it, uLight, &wi, &lightPdf, &visibility);
    /* TODO
    VLOG(2) << "EstimateDirect uLight:" << uLight << " -> Li: " << Li << ", wi: "
        << wi << ", pdf: " << lightPdf;
    */
    if (lightPdf > FLOAT_0 && !Li.IsBlack())
    {
        // Compute BSDF or phase function's value for light sample
        core::color::Spectrum f;
        if (it.IsSurfaceInteraction())
        {
            // Evaluate BSDF for light sampling strategy
            const core::interaction::SurfaceInteraction &isect = (const core::interaction::SurfaceInteraction &)it;
            f = isect.bsdf->f(isect.wo, wi, bsdfFlags) * AbsDot(wi, isect.shading.n);
            scatteringPdf = isect.bsdf->Pdf(isect.wo, wi, bsdfFlags);
            /* TODO
            VLOG(2) << "  surf f*dot :" << f << ", scatteringPdf: " << scatteringPdf;
            */
        }
        else
        {
            // Evaluate phase function for light sampling strategy
            const core::interaction::MediumInteraction &mi = (const core::interaction::MediumInteraction &)it;
            Float p = mi.phase->p(mi.wo, wi);
            f = core::color::Spectrum(p);
            scatteringPdf = p;
            /* TODO
            VLOG(2) << "  medium p: " << p;
            */
        }
        if (!f.IsBlack())
        {
            // Compute effect of visibility for light source sample
            if (handleMedia)
            {
                Li *= visibility.Tr(scene, sampler);
                /* TODO
                VLOG(2) << "  after Tr, Li: " << Li;
                */
            }
            else
            {
                if (!visibility.Unoccluded(scene))
                {
                    /* TODO
                    VLOG(2) << "  shadow ray blocked";
                    Li = core::color::Spectrum(FLOAT_0);
                    */
                }
                /* TODO
                else
                    VLOG(2) << "  shadow ray unoccluded";
                */
            }

            // Add light's contribution to reflected radiance
            if (!Li.IsBlack())
            {
                if (core::light::IsDeltaLight(light.flags))
                {
                    Ld += f * Li / lightPdf;
                }
                else
                {
                    Float weight = core::sampler::PowerHeuristic(1, lightPdf, 1, scatteringPdf);
                    Ld += f * Li * weight / lightPdf;
                }
            }
        }
    }

    // Sample BSDF with multiple importance sampling
    if (!core::light::IsDeltaLight(light.flags))
    {
        core::color::Spectrum f;
        bool sampledSpecular = false;
        if (it.IsSurfaceInteraction())
        {
            // Sample scattered direction for surface interactions
            core::bxdf::BxDFType sampledType;
            const core::interaction::SurfaceInteraction &isect = (const core::interaction::SurfaceInteraction &)it;
            f = isect.bsdf->Sample_f(isect.wo, &wi, uScattering, &scatteringPdf,
                bsdfFlags, &sampledType);
            f *= AbsDot(wi, isect.shading.n);
            sampledSpecular = (sampledType & core::bxdf::BxDFType::BSDF_SPECULAR) != 0;
        }
        else
        {
            // Sample scattered direction for medium interactions
            const core::interaction::MediumInteraction &mi = (const core::interaction::MediumInteraction &)it;
            Float p = mi.phase->Sample_p(mi.wo, &wi, uScattering);
            f = core::color::Spectrum(p);
            scatteringPdf = p;
        }

        /* TODO
        VLOG(2) << "  BSDF / phase sampling f: " << f << ", scatteringPdf: " <<
            scatteringPdf;
        */
        if (!f.IsBlack() && scatteringPdf > FLOAT_0)
        {
            // Account for light contributions along sampled direction _wi_
            Float weight = FLOAT_1;
            if (!sampledSpecular)
            {
                lightPdf = light.Pdf_Li(it, wi);
                if (FLOAT_0 == lightPdf)
                {
                    return Ld;
                }
                weight = core::sampler::PowerHeuristic(1, scatteringPdf, 1, lightPdf);
            }

            // Find intersection and compute transmittance
            core::interaction::SurfaceInteraction lightIsect;
            common::math::Rayf ray = it.SpawnRay(wi);
            core::color::Spectrum Tr(FLOAT_1);
            bool foundSurfaceInteraction =
                handleMedia ? scene.IntersectTr(ray, sampler, &lightIsect, &Tr)
                : scene.Intersect(ray, &lightIsect);

            // Add light contribution from material sampling
            core::color::Spectrum Li(FLOAT_0);
            if (foundSurfaceInteraction)
            {
                if (lightIsect.primitive->GetAreaLight() == &light)
                {
                    Li = lightIsect.Le(-wi);
                }
            }
            else
            {
                Li = light.Le(ray);
            }
            if (!Li.IsBlack())
            {
                Ld += f * Li * Tr * weight / scatteringPdf;
            }
        }
    }
    return Ld;
}

std::unique_ptr<core::sampler::Distribution1D> ComputeLightPowerDistribution(
    const core::scene::Scene &scene)
{
    if (scene.lights.empty())
    {
        return nullptr;
    }
    std::vector<Float> lightPower;
    for (const auto &light : scene.lights)
    {
        lightPower.push_back(light->Power().y());
    }
    return std::unique_ptr<core::sampler::Distribution1D>(
        new core::sampler::Distribution1D(&lightPower[0], lightPower.size()));
}


}
}