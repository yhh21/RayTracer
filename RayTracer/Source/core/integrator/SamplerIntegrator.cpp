#include "SamplerIntegrator.h"
#include "../bxdf/BxDF.h"
#include "../bxdf/BSDF.h"
#include "../color/Spectrum.h"
#include "../film/Film.h"
#include "../film/FilmTile.h"
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
#include "../../common/tool/MultiThread.h"

namespace core
{
namespace integrator
{


void SamplerIntegrator::Render(const core::scene::Scene &scene)
{
    Preprocess(scene, *sampler);
    // Render image tiles in parallel

    // Compute number of tiles, _nTiles_, to use for parallel rendering
    common::math::Bounds2i sampleBounds = camera->film->GetSampleBounds();
    common::math::Vec2i sampleExtent = sampleBounds.Diagonal();
    const int tileSize = 16;
    common::math::Vec2i nTiles((sampleExtent.x + tileSize - 1) / tileSize,
        (sampleExtent.y + tileSize - 1) / tileSize);
    //ProgressReporter reporter(nTiles.x * nTiles.y, "Rendering");

    {
        common::tool::ParallelFor2D([&](common::math::Vec2i tile)
        {
            // Render section of image corresponding to _tile_

            // Allocate _MemoryArena_ for tile
            common::tool::MemoryArena arena;

            // Get sampler instance for tile
            int seed = tile.y * nTiles.x + tile.x;
            std::unique_ptr<core::sampler::Sampler> tileSampler = sampler->Clone(seed);

            // Compute sample bounds for tile
            int x0 = sampleBounds.point_min.x + tile.x * tileSize;
            int x1 = (std::min)(x0 + tileSize, sampleBounds.point_max.x);
            int y0 = sampleBounds.point_min.y + tile.y * tileSize;
            int y1 = (std::min)(y0 + tileSize, sampleBounds.point_max.y);
            common::math::Bounds2i tileBounds(common::math::Vec2i(x0, y0), common::math::Vec2i(x1, y1));
            //LOG(INFO) << "Starting image tile " << tileBounds;

            // Get _FilmTile_ for tile
            std::unique_ptr<core::film::FilmTile> filmTile = camera->film->GetFilmTile(tileBounds);

            // Loop over pixels in tile to render them
            for (common::math::Vec2i pixel : tileBounds)
            {
                {
                    //ProfilePhase pp(Prof::StartPixel);
                    tileSampler->StartPixel(pixel);
                }

                // Do this check after the StartPixel() call; this keeps
                // the usage of RNG values from (most) Samplers that use
                // RNGs consistent, which improves reproducability /
                // debugging.
                if (!InsideExclusive(pixel, pixelBounds))
                {
                    continue;
                }

                do
                {
                    // Initialize _CameraSample_ for current sample
                    core::camera::CameraSample cameraSample = tileSampler->GetCameraSample(pixel);

                    // Generate camera ray for current sample
                    common::math::RayDifferentialf ray;
                    Float rayWeight = camera->GenerateRayDifferential(cameraSample, &ray);
                    ray.ScaleDifferentials(FLOAT_1
                        / std::sqrt(static_cast<Float>(tileSampler->samples_per_pixel)));
                    //++nCameraRays;

                    // Evaluate radiance along camera ray
                    core::color::Spectrum L(FLOAT_0);
                    if (rayWeight > FLOAT_0)
                    {
                        L = Li(ray, scene, *tileSampler, arena);
                    }

                    // Issue warning if unexpected radiance value returned
                    if (L.HasNaNs())
                    {
                        /* TODO
                        LOG(ERROR) << StringPrintf(
                            "Not-a-number radiance value returned "
                            "for pixel (%d, %d), sample %d. Setting to black.",
                            pixel.x, pixel.y,
                            (int)tileSampler->CurrentSampleNumber());
                        */
                        L = core::color::Spectrum(FLOAT_0);
                    }
                    else if (L.y() < static_cast<Float>(-1e-5))
                    {
                        /* TODO
                        LOG(ERROR) << StringPrintf(
                            "Negative luminance value, %f, returned "
                            "for pixel (%d, %d), sample %d. Setting to black.",
                            L.y(), pixel.x, pixel.y,
                            (int)tileSampler->CurrentSampleNumber());
                        L = core::color::Spectrum(FLOAT_0);
                        */
                    }
                    else if (std::isinf(L.y()))
                    {
                        /* TODO
                        LOG(ERROR) << StringPrintf(
                            "Infinite luminance value returned "
                            "for pixel (%d, %d), sample %d. Setting to black.",
                            pixel.x, pixel.y,
                            (int)tileSampler->CurrentSampleNumber());
                        */
                        L = core::color::Spectrum(FLOAT_0);
                    }
                    /* TODO
                    VLOG(1) << "core::camera::Camera sample: " << cameraSample << " -> ray: " <<
                        ray << " -> L = " << L;
                    */

                    // Add camera ray's contribution to image
                    filmTile->AddSample(cameraSample.pFilm, L, rayWeight);

                    // Free _MemoryArena_ memory from computing image sample
                    // value
                    arena.Reset();
                }
                while (tileSampler->StartNextSample());
            }
            /* TODO
            LOG(INFO) << "Finished image tile " << tileBounds;
            */

            // Merge image tile into _Film_
            camera->film->MergeFilmTile(std::move(filmTile));
            //reporter.Update();
        }, nTiles);
        //reporter.Done();
    }
    /* TODO
    LOG(INFO) << "Rendering finished";
    */

    // Save final image after rendering
    camera->film->WriteImage();
}

core::color::Spectrum SamplerIntegrator::SpecularReflect(
    const common::math::RayDifferentialf &ray, const core::interaction::SurfaceInteraction &isect,
    const core::scene::Scene &scene, core::sampler::Sampler &sampler, common::tool::MemoryArena &arena, int depth) const
{
    // Compute specular reflection direction _wi_ and BSDF value
    common::math::Vec3f wo = isect.wo, wi;
    Float pdf;
    core::bxdf::BxDFType type = 
        core::bxdf::BxDFType(core::bxdf::BxDFType::BSDF_REFLECTION | core::bxdf::BxDFType::BSDF_SPECULAR);
    core::color::Spectrum f = isect.bsdf->Sample_f(wo, &wi, sampler.Get2D(), &pdf, type);

    // Return contribution of specular reflection
    const common::math::Vec3f &ns = isect.shading.n;
    if (pdf > FLOAT_0 && !f.IsBlack() && AbsDot(wi, ns) != FLOAT_0)
    {
        // Compute ray differential _rd_ for specular reflection
        common::math::RayDifferentialf rd = isect.SpawnRay(wi);
        if (ray.has_differentials)
        {
            rd.has_differentials = true;
            rd.rx_origin = isect.p + isect.dp_dx;
            rd.ry_origin = isect.p + isect.dp_dy;
            // Compute differential reflected directions
            common::math::Vec3f dndx = isect.shading.dn_du * isect.du_dx +
                isect.shading.dn_dv * isect.dv_dx;
            common::math::Vec3f dndy = isect.shading.dn_du * isect.du_dy +
                isect.shading.dn_dv * isect.dv_dy;
            common::math::Vec3f dwodx = -ray.rx_direction - wo,
                dwody = -ray.ry_direction - wo;
            Float dDNdx = Dot(dwodx, ns) + Dot(wo, dndx);
            Float dDNdy = Dot(dwody, ns) + Dot(wo, dndy);
            rd.rx_direction =
                wi - dwodx + FLOAT_2 * common::math::Vec3f(Dot(wo, ns) * dndx + dDNdx * ns);
            rd.ry_direction =
                wi - dwody + FLOAT_2 * common::math::Vec3f(Dot(wo, ns) * dndy + dDNdy * ns);
        }
        return f * Li(rd, scene, sampler, arena, depth + 1) * AbsDot(wi, ns) /
            pdf;
    }
    else
        return core::color::Spectrum(FLOAT_0);
}

core::color::Spectrum SamplerIntegrator::SpecularTransmit(
    const common::math::RayDifferentialf &ray, const core::interaction::SurfaceInteraction &isect,
    const core::scene::Scene &scene, core::sampler::Sampler &sampler,
    common::tool::MemoryArena &arena, int depth) const
{
    common::math::Vec3f wo = isect.wo, wi;
    Float pdf;
    const common::math::Vec3f &p = isect.p;
    const core::bxdf::BSDF &bsdf = *isect.bsdf;
    core::color::Spectrum f = bsdf.Sample_f(wo, &wi, sampler.Get2D(), &pdf,
        core::bxdf::BxDFType(core::bxdf::BxDFType::BSDF_TRANSMISSION | core::bxdf::BxDFType::BSDF_SPECULAR));
    core::color::Spectrum L = core::color::Spectrum(FLOAT_0);
    common::math::Vec3f ns = isect.shading.n;
    if (pdf > FLOAT_0 && !f.IsBlack() && AbsDot(wi, ns) != FLOAT_0)
    {
        // Compute ray differential _rd_ for specular transmission
        common::math::RayDifferentialf rd = isect.SpawnRay(wi);
        if (ray.has_differentials)
        {
            rd.has_differentials = true;
            rd.rx_origin = p + isect.dp_dx;
            rd.ry_origin = p + isect.dp_dy;

            common::math::Vec3f dndx = isect.shading.dn_du * isect.du_dx +
                isect.shading.dn_dv * isect.dv_dx;
            common::math::Vec3f dndy = isect.shading.dn_du * isect.du_dy +
                isect.shading.dn_dv * isect.dv_dy;

            // The BSDF stores the IOR of the interior of the object being
            // intersected.  Compute the relative IOR by first out by
            // assuming that the ray is entering the object.
            Float eta = FLOAT_1 / bsdf.eta;
            if (Dot(wo, ns) < FLOAT_0)
            {
                // If the ray isn't entering, then we need to invert the
                // relative IOR and negate the normal and its derivatives.
                eta = FLOAT_1 / eta;
                ns = -ns;
                dndx = -dndx;
                dndy = -dndy;
            }

            /*
              Notes on the derivation:
              - pbrt computes the refracted ray as: \wi = -\eta \omega_o + [ \eta (\wo \cdot \N) - \cos \theta_t ] \N
                It flips the normal to lie in the same hemisphere as \wo, and then \eta is the relative IOR from
                \wo's medium to \wi's medium.
              - If we denote the term in brackets by \mu, then we have: \wi = -\eta \omega_o + \mu \N
              - Now let's take the partial derivative. (We'll use "d" for \partial in the following for brevity.)
                We get: -\eta d\omega_o / dx + \mu dN/dx + d\mu/dx N.
              - We have the values of all of these except for d\mu/dx (using bits from the derivation of specularly
                reflected ray deifferentials).
              - The first term of d\mu/dx is easy: \eta d(\wo \cdot N)/dx. We already have d(\wo \cdot N)/dx.
              - The second term takes a little more work. We have:
                 \cos \theta_i = \sqrt{1 - \eta^2 (1 - (\wo \cdot N)^2)}.
                 Starting from (\wo \cdot N)^2 and reading outward, we have \cos^2 \theta_o, then \sin^2 \theta_o,
                 then \sin^2 \theta_i (via Snell's law), then \cos^2 \theta_i and then \cos \theta_i.
              - Let's take the partial derivative of the sqrt expression. We get:
                1 / 2 * 1 / \cos \theta_i * d/dx (1 - \eta^2 (1 - (\wo \cdot N)^2)).
              - That partial derivatve is equal to:
                d/dx \eta^2 (\wo \cdot N)^2 = 2 \eta^2 (\wo \cdot N) d/dx (\wo \cdot N).
              - Plugging it in, we have d\mu/dx =
                \eta d(\wo \cdot N)/dx - (\eta^2 (\wo \cdot N) d/dx (\wo \cdot N))/(-\wi \cdot N).
             */
            common::math::Vec3f dwodx = -ray.rx_direction - wo,
                dwody = -ray.ry_direction - wo;
            Float dDNdx = Dot(dwodx, ns) + Dot(wo, dndx);
            Float dDNdy = Dot(dwody, ns) + Dot(wo, dndy);

            Float mu = eta * Dot(wo, ns) - AbsDot(wi, ns);
            Float dmudx = (eta - (eta * eta * Dot(wo, ns)) / AbsDot(wi, ns)) * dDNdx;
            Float dmudy = (eta - (eta * eta * Dot(wo, ns)) / AbsDot(wi, ns)) * dDNdy;

            rd.rx_direction = wi - eta * dwodx + common::math::Vec3f(mu * dndx + dmudx * ns);
            rd.ry_direction = wi - eta * dwody + common::math::Vec3f(mu * dndy + dmudy * ns);
        }
        L = f * Li(rd, scene, sampler, arena, depth + 1) * AbsDot(wi, ns) / pdf;
    }
    return L;
}


}
}