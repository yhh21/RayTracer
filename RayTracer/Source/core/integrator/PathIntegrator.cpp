#include "PathIntegrator.h"
#include "LightDistribution.h"
#include "../bxdf/BxDF.h"
#include "../bxdf/BSDF.h"
#include "../bxdf/BSSRDF.h"
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


PathIntegrator::PathIntegrator(int maxDepth,
    std::shared_ptr<const core::camera::Camera> camera,
    std::shared_ptr<core::sampler::Sampler> sampler,
    const common::math::Bounds2i &pixelBounds, Float rrThreshold,
    const std::string &lightSampleStrategy)
    : SamplerIntegrator(camera, sampler, pixelBounds),
    maxDepth(maxDepth),
    rrThreshold(rrThreshold),
    lightSampleStrategy(lightSampleStrategy)
{}

void PathIntegrator::Preprocess(const core::scene::Scene &scene, core::sampler::Sampler &sampler)
{
    lightDistribution = CreateLightSampleDistribution(lightSampleStrategy, scene);
}

core::color::Spectrum PathIntegrator::Li(const common::math::RayDifferentialf &r, const core::scene::Scene &scene,
    core::sampler::Sampler &sampler, common::tool::MemoryArena &arena,
    int depth) const
{
    //ProfilePhase p(Prof::SamplerIntegratorLi);
    core::color::Spectrum L(FLOAT_0), beta(FLOAT_1);
    common::math::RayDifferentialf ray(r);
    bool specularBounce = false;
    int bounces;
    // Added after book publication: etaScale tracks the accumulated effect
    // of radiance scaling due to rays passing through refractive
    // boundaries (see the derivation on p. 527 of the third edition). We
    // track this value in order to remove it from beta when we apply
    // Russian roulette; this is worthwhile, since it lets us sometimes
    // avoid terminating refracted rays that are about to be refracted back
    // out of a medium and thus have their beta value increased.
    Float etaScale = FLOAT_1;

    for (bounces = 0;; ++bounces)
    {
        // Find next path vertex and accumulate contribution
        /* TODO
        VLOG(2) << "Path tracer bounce " << bounces << ", current L = " << L
            << ", beta = " << beta;
        */

        // Intersect _ray_ with scene and store intersection in _isect_
        core::interaction::SurfaceInteraction isect;
        bool foundIntersection = scene.Intersect(ray, &isect);

        // Possibly add emitted light at intersection
        if (0 == bounces || specularBounce)
        {
            // Add emitted light at path vertex or from the environment
            if (foundIntersection)
            {
                L += beta * isect.Le(-ray.dir);
                /* TODO
                VLOG(2) << "Added Le -> L = " << L;
                */
            }
            else
            {
                for (const auto &light : scene.infiniteLights)
                    L += beta * light->Le(ray);
                /* TODO
                VLOG(2) << "Added infinite area lights -> L = " << L;
                */
            }
        }

        // Terminate path if ray escaped or _maxDepth_ was reached
        if (!foundIntersection || bounces >= maxDepth) break;

        // Compute scattering functions and skip over medium boundaries
        isect.ComputeScatteringFunctions(ray, arena, true);
        if (!isect.bsdf)
        {
            /* TODO
            VLOG(2) << "Skipping intersection due to null bsdf";
            */
            ray = isect.SpawnRay(ray.dir);
            --bounces;
            continue;
        }

        const core::sampler::Distribution1D *distrib = lightDistribution->Lookup(isect.p);

        // Sample illumination from lights to find path contribution.
        // (But skip this for perfectly specular BSDFs.)
        if (isect.bsdf->NumComponents(
            core::bxdf::BxDFType(core::bxdf::BxDFType::BSDF_ALL & ~core::bxdf::BxDFType::BSDF_SPECULAR)) > 0)
        {
            //++totalPaths;
            core::color::Spectrum Ld = beta * UniformSampleOneLight(isect, scene, arena,
                sampler, false, distrib);
            /* TODO
            VLOG(2) << "Sampled direct lighting Ld = " << Ld;
            */
            if (Ld.IsBlack())
            {
                //++zeroRadiancePaths;
            }
            CHECK_GE(Ld.y(), FLOAT_0);
            L += Ld;
        }

        // Sample BSDF to get new path direction
        common::math::Vec3f wo = -ray.dir, wi;
        Float pdf;
        core::bxdf::BxDFType flags;
        core::color::Spectrum f = isect.bsdf->Sample_f(wo, &wi, sampler.Get2D(), &pdf,
            core::bxdf::BxDFType::BSDF_ALL, &flags);
        /* TODO
        VLOG(2) << "Sampled BSDF, f = " << f << ", pdf = " << pdf;
        */
        if (f.IsBlack() || FLOAT_0 == pdf)
        {
            break;
        }
        beta *= f * AbsDot(wi, isect.shading.n) / pdf;
        /* TODO
        VLOG(2) << "Updated beta = " << beta;
        */
        CHECK_GE(beta.y(), FLOAT_0);
        CHECK(!std::isinf(beta.y()));
        specularBounce = (flags & core::bxdf::BxDFType::BSDF_SPECULAR) != 0;
        if ((flags & core::bxdf::BxDFType::BSDF_SPECULAR) && (flags & core::bxdf::BxDFType::BSDF_TRANSMISSION))
        {
            Float eta = isect.bsdf->eta;
            // Update the term that tracks radiance scaling for refraction
            // depending on whether the ray is entering or leaving the
            // medium.
            etaScale *= (Dot(wo, isect.n) > FLOAT_0) ? (eta * eta) : FLOAT_1 / (eta * eta);
        }
        ray = isect.SpawnRay(wi);

        // Account for subsurface scattering, if applicable
        if (isect.bssrdf && (flags & core::bxdf::BxDFType::BSDF_TRANSMISSION))
        {
            // Importance sample the BSSRDF
            core::interaction::SurfaceInteraction pi;
            core::color::Spectrum S = isect.bssrdf->Sample_S(
                scene, sampler.Get1D(), sampler.Get2D(), arena, &pi, &pdf);
            CHECK(!std::isinf(beta.y()));
            if (S.IsBlack() || FLOAT_0 == pdf)
            {
                break;
            }
            beta *= S / pdf;

            // Account for the direct subsurface scattering component
            L += beta * UniformSampleOneLight(pi, scene, arena, sampler, false,
                lightDistribution->Lookup(pi.p));

            // Account for the indirect subsurface scattering component
            core::color::Spectrum f = pi.bsdf->Sample_f(pi.wo, &wi, sampler.Get2D(), &pdf,
                core::bxdf::BxDFType::BSDF_ALL, &flags);
            if (f.IsBlack() || FLOAT_0 == pdf)
            {
                break;
            }
            beta *= f * AbsDot(wi, pi.shading.n) / pdf;
            CHECK(!std::isinf(beta.y()));
            specularBounce = (flags & core::bxdf::BxDFType::BSDF_SPECULAR) != 0;
            ray = pi.SpawnRay(wi);
        }

        // Possibly terminate the path with Russian roulette.
        // Factor out radiance scaling due to refraction in rrBeta.
        core::color::Spectrum rrBeta = beta * etaScale;
        if (rrBeta.MaxComponentValue() < rrThreshold && bounces > 3)
        {
            Float q = (std::max)(static_cast<Float>(0.05F), FLOAT_1 - rrBeta.MaxComponentValue());
            if (sampler.Get1D() < q)
            {
                break;
            }
            beta /= FLOAT_1 - q;
            CHECK(!std::isinf(beta.y()));
        }
    }
    //ReportValue(pathLength, bounces);
    return L;
}

/* TODO
PathIntegrator *CreatePathIntegrator(const ParamSet &params,
    std::shared_ptr<core::sampler::Sampler> sampler,
    std::shared_ptr<const core::camera::Camera> camera)
{
    int maxDepth = params.FindOneInt("maxdepth", 5);
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
            if (pixelBounds.Area() == 0)
                Error("Degenerate \"pixelbounds\" specified.");
        }
    }
    Float rrThreshold = params.FindOneFloat("rrthreshold", 1.);
    std::string lightStrategy =
        params.FindOneString("lightsamplestrategy", "spatial");
    return new PathIntegrator(maxDepth, camera, sampler, pixelBounds,
        rrThreshold, lightStrategy);
}
*/


}
}