#include "InfiniteAreaLight.h"
#include "VisibilityTester.h"
#include "../../common/math/Constants.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/RayDifferential.h"
#include "../bxdf/BxDF.h"
#include "../sampler/Sampling.h"

namespace core
{
namespace light
{


InfiniteAreaLight::InfiniteAreaLight(const common::math::Transformf &LightToWorld,
    const core::color::Spectrum &L, int nSamples,
    const std::string &texmap)
    : Light(static_cast<int>(LightFlags::Infinite), LightToWorld, interaction::MediumInterface(),
        nSamples)
{
    // Read texel data from _texmap_ and initialize _Lmap_
    common::math::Vec2i resolution;
    std::unique_ptr<core::color::RGBSpectrum[]> texels(nullptr);
    if (texmap != "")
    {
        /* TODO
        texels = ReadImage(texmap, &resolution);
        if (texels)
        {
            for (int i = 0; i < resolution.x * resolution.y; ++i)
            {
                texels[i] *= L.ToRGBSpectrum();
            }
        }
        */
    }
    if (!texels)
    {
        resolution.x = resolution.y = 1;
        texels = std::unique_ptr<core::color::RGBSpectrum[]>(new core::color::RGBSpectrum[1]);
        texels[0] = L.ToRGBSpectrum();
    }
    Lmap.reset(new common::tool::MIPMap<core::color::RGBSpectrum>(resolution, texels.get()));

    // Initialize sampling PDFs for infinite area light

    // Compute scalar-valued image _img_ from environment map
    int width = 2 * Lmap->Width(), height = 2 * Lmap->Height();
    std::unique_ptr<Float[]> img(new Float[width * height]);
    float fwidth = FLOAT_INV_2 / (std::min)(width, height);
    common::tool::ParallelFor( [&](int64_t v)
    {
        Float vp = (v + FLOAT_INV_2) / (Float)height;
        Float sinTheta = std::sin(common::math::PI * (v + FLOAT_INV_2) / height);
        for (int u = 0; u < width; ++u)
        {
            Float up = (u + FLOAT_INV_2) / (Float)width;
            img[u + v * width] = Lmap->Lookup(common::math::Vec2f(up, vp), fwidth).y();
            img[u + v * width] *= sinTheta;
        }
    }, height, 32);

    // Compute sampling distributions for rows and columns of image
    distribution.reset(new sampler::Distribution2D(img.get(), width, height));
}

core::color::Spectrum InfiniteAreaLight::Power() const
{
    return common::math::PI * worldRadius * worldRadius *
        core::color::Spectrum(Lmap->Lookup(common::math::Vec2f(FLOAT_INV_2, FLOAT_INV_2), FLOAT_INV_2),
            core::color::SpectrumType::Illuminant);
}

core::color::Spectrum InfiniteAreaLight::Le(const common::math::RayDifferentialf &ray) const
{
    common::math::Vec3f w = Normalize(WorldToLight(ray.dir));
    common::math::Vec2f st(SphericalPhi(w) * common::math::INV_TWO_PI, SphericalTheta(w) * common::math::INV_PI);
    return core::color::Spectrum(Lmap->Lookup(st), core::color::SpectrumType::Illuminant);
}

core::color::Spectrum InfiniteAreaLight::Sample_Li(const interaction::Interaction &ref, const common::math::Vec2f &u,
    common::math::Vec3f *wi, Float *pdf,
    VisibilityTester *vis) const
{
    //ProfilePhase _(Prof::LightSample);
    // Find $(u,v)$ sample coordinates in infinite light texture
    Float mapPdf;
    common::math::Vec2f uv = distribution->SampleContinuous(u, &mapPdf);
    if (FLOAT_0 == mapPdf)
    {
        return core::color::Spectrum(FLOAT_0);
    }

    // Convert infinite light sample point to direction
    Float theta = uv[1] * common::math::PI, phi = uv[0] * FLOAT_2 * common::math::PI;
    Float cosTheta = std::cos(theta), sinTheta = std::sin(theta);
    Float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
    *wi = LightToWorld(common::math::Vec3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta));

    // Compute PDF for sampled infinite light direction
    *pdf = mapPdf / (FLOAT_2 * common::math::PI * common::math::PI * sinTheta);
    if (FLOAT_0 == sinTheta)
    {
        *pdf = FLOAT_0;
    }

    // Return radiance value for infinite light direction
    *vis = VisibilityTester(ref, interaction::Interaction(ref.p + *wi * (FLOAT_2 * worldRadius),
        ref.time, medium_interface));
    return core::color::Spectrum(Lmap->Lookup(uv), core::color::SpectrumType::Illuminant);
}

Float InfiniteAreaLight::Pdf_Li(const interaction::Interaction &, const common::math::Vec3f &w) const
{
    //ProfilePhase _(Prof::LightPdf);
    common::math::Vec3f wi = WorldToLight(w);
    Float theta = SphericalTheta(wi), phi = SphericalPhi(wi);
    Float sinTheta = std::sin(theta);
    if (FLOAT_0 == sinTheta)
    {
        return FLOAT_0;
    }
    return distribution->Pdf(common::math::Vec2f(phi * common::math::INV_TWO_PI, theta * common::math::INV_PI)) /
        (FLOAT_2 * common::math::PI * common::math::PI * sinTheta);
}

core::color::Spectrum InfiniteAreaLight::Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2,
    Float time, common::math::Rayf *ray, common::math::Vec3f *nLight,
    Float *pdfPos, Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightSample);
    // Compute direction for infinite light sample ray
    common::math::Vec2f u = u1;

    // Find $(u,v)$ sample coordinates in infinite light texture
    Float mapPdf;
    common::math::Vec2f uv = distribution->SampleContinuous(u, &mapPdf);
    if (FLOAT_0 == mapPdf)
    {
        return core::color::Spectrum(FLOAT_0);
    }
    Float theta = uv[1] * common::math::PI, phi = uv[0] * FLOAT_2 * common::math::PI;
    Float cosTheta = std::cos(theta), sinTheta = std::sin(theta);
    Float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
    common::math::Vec3f d = -LightToWorld(common::math::Vec3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta));
    *nLight = (common::math::Vec3f)d;

    // Compute origin for infinite light sample ray
    common::math::Vec3f v1, v2;
    CoordinateSystem(-d, &v1, &v2);
    common::math::Vec2f cd = sampler::ConcentricSampleDisk(u2);
    common::math::Vec3f pDisk = worldCenter + worldRadius * (cd.x * v1 + cd.y * v2);
    *ray = common::math::Rayf(pDisk + worldRadius * -d, d, (std::numeric_limits<Float>::max)(), time);

    // Compute _InfiniteAreaLight_ ray PDFs
    *pdfDir = FLOAT_0 == sinTheta ? FLOAT_0 : mapPdf / (FLOAT_2 * common::math::PI * common::math::PI * sinTheta);
    *pdfPos = FLOAT_1 / (common::math::PI * worldRadius * worldRadius);
    return core::color::Spectrum(Lmap->Lookup(uv), core::color::SpectrumType::Illuminant);
}

void InfiniteAreaLight::Pdf_Le(const common::math::Rayf &ray, const common::math::Vec3f &, Float *pdfPos,
    Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightPdf);
    common::math::Vec3f d = -WorldToLight(ray.dir);
    Float theta = SphericalTheta(d), phi = SphericalPhi(d);
    common::math::Vec2f uv(phi * common::math::INV_TWO_PI, theta * common::math::INV_PI);
    Float mapPdf = distribution->Pdf(uv);
    *pdfDir = mapPdf / (FLOAT_2 * common::math::PI * common::math::PI * std::sin(theta));
    *pdfPos = FLOAT_1 / (common::math::PI * worldRadius * worldRadius);
}

/* TODO
std::shared_ptr<InfiniteAreaLight> CreateInfiniteLight(
    const common::math::Transformf &light2world, const ParamSet &paramSet)
{
    core::color::Spectrum L = paramSet.FindOneSpectrum("L", core::color::Spectrum(1.0));
    core::color::Spectrum sc = paramSet.FindOneSpectrum("scale", core::color::Spectrum(1.0));
    std::string texmap = paramSet.FindOneFilename("mapname", "");
    int nSamples = paramSet.FindOneInt("samples",
        paramSet.FindOneInt("nsamples", 1));
    if (PbrtOptions.quickRender) nSamples = (std::min)(1, nSamples / 4);
    return std::make_shared<InfiniteAreaLight>(light2world, L * sc, nSamples,
        texmap);
}
*/


}
}