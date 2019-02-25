#include "ProjectionLight.h"
#include "VisibilityTester.h"
#include "../../common/math/Constants.h"
#include "../../common/math/Vec2.h"
#include "../bxdf/BxDF.h"
#include "../sampler/Sampling.h"

namespace core
{
namespace light
{


ProjectionLight::ProjectionLight(const common::math::Transformf &LightToWorld,
    const interaction::MediumInterface &medium_interface,
    const core::color::Spectrum &I, const std::string &texname,
    Float fov)
    : Light(static_cast<int>(LightFlags::DeltaPosition), LightToWorld, medium_interface),
    pLight(LightToWorld(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0))),
    I(I)
{
    // Create _ProjectionLight_ MIP map
    common::math::Vec2i resolution;
    /* TODO
    std::unique_ptr<core::color::RGBSpectrum[]> texels = ReadImage(texname, &resolution);
    if (texels)
    {
        projectionMap.reset(new MIPMap<core::color::RGBSpectrum>(resolution, texels.get()));
    }
    */

    // Initialize _ProjectionLight_ projection matrix
    Float aspect = nullptr != projectionMap
        ? (static_cast<Float>(resolution.x) / static_cast<Float>(resolution.y)) : FLOAT_1;
    if (aspect > FLOAT_1)
    {
        screenBounds = common::math::Bounds2f(common::math::Vec2f(-aspect, -FLOAT_1)
            , common::math::Vec2f(aspect, FLOAT_1));
    }
    else
    {
        screenBounds = common::math::Bounds2f(common::math::Vec2f(-FLOAT_1, -FLOAT_1 / aspect)
                , common::math::Vec2f(FLOAT_1, FLOAT_1 / aspect));
    }
    hither = static_cast<Float>(1e-3f);
    yon = static_cast<Float>(1e30f);
    lightProjection = common::math::Perspective(fov, hither, yon);

    common::math::Transformf screenToLight = Inverse(lightProjection);
    common::math::Vec3f pCorner(screenBounds.point_max.x, screenBounds.point_max.y, FLOAT_0);
    common::math::Vec3f wCorner = Normalize(common::math::Vec3f(screenToLight(pCorner)));
    cosTotalWidth = wCorner.z;
}

core::color::Spectrum ProjectionLight::Sample_Li(const interaction::Interaction &ref, const common::math::Vec2f &u,
    common::math::Vec3f *wi, Float *pdf,
    VisibilityTester *vis) const
{
    //ProfilePhase _(Prof::LightSample);
    *wi = Normalize(pLight - ref.p);
    *pdf = FLOAT_1;
    *vis = VisibilityTester(ref, interaction::Interaction(pLight, ref.time, medium_interface));
    return I * Projection(-*wi) / DistanceSquared(pLight, ref.p);
}

core::color::Spectrum ProjectionLight::Projection(const common::math::Vec3f &w) const
{
    common::math::Vec3f wl = WorldToLight(w);
    // Discard directions behind projection light
    if (wl.z < hither)
    {
        return core::color::Spectrum(FLOAT_0);
    }

    // Project point onto projection plane and compute light
    common::math::Vec3f p = lightProjection(common::math::Vec3f(wl.x, wl.y, wl.z));
    if (!Inside(common::math::Vec2f(p.x, p.y), screenBounds))
    {
        return core::color::Spectrum(FLOAT_0);
    }
    if (!projectionMap)
    {
        return core::color::Spectrum(FLOAT_1);
    }
    common::math::Vec2f st = common::math::Vec2f(screenBounds.Offset(common::math::Vec2f(p.x, p.y)));
    return core::color::Spectrum(projectionMap->Lookup(st), core::color::SpectrumType::Illuminant);
}

core::color::Spectrum ProjectionLight::Power() const
{
    return (projectionMap
        ? core::color::Spectrum(projectionMap->Lookup(common::math::Vec2f(FLOAT_INV_2, FLOAT_INV_2), FLOAT_INV_2),
            core::color::SpectrumType::Illuminant)
        : core::color::Spectrum(FLOAT_1)) *
        I * FLOAT_2 * common::math::PI * (FLOAT_1 - cosTotalWidth);
}

Float ProjectionLight::Pdf_Li(const interaction::Interaction &, const common::math::Vec3f &) const
{
    return FLOAT_0;
}

core::color::Spectrum ProjectionLight::Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2,
    Float time, common::math::Rayf *ray, common::math::Vec3f *nLight,
    Float *pdfPos, Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightSample);
    common::math::Vec3f v = sampler::UniformSampleCone(u1, cosTotalWidth);
    *ray = common::math::Rayf(pLight, LightToWorld(v), (std::numeric_limits<Float>::max)(), FLOAT_0,
        time, medium_interface.inside);
    *nLight = (common::math::Vec3f)ray->dir;
    *pdfPos = FLOAT_1;
    *pdfDir = sampler::UniformConePdf(cosTotalWidth);
    return I * Projection(ray->dir);
}

void ProjectionLight::Pdf_Le(const common::math::Rayf &ray, const common::math::Vec3f &, Float *pdfPos,
    Float *pdfDir) const
{
    //ProfilePhase _(Prof::LightPdf);
    *pdfPos = FLOAT_0;
    *pdfDir = (bxdf::CosTheta(WorldToLight(ray.dir)) >= cosTotalWidth)
        ? sampler::UniformConePdf(cosTotalWidth)
        : FLOAT_0;
}

/* TODO
std::shared_ptr<ProjectionLight> CreateProjectionLight(
    const common::math::Transformf &light2world, const Medium *medium,
    const ParamSet &paramSet)
{
    core::color::Spectrum I = paramSet.FindOneSpectrum("I", core::color::Spectrum(1.0));
    core::color::Spectrum sc = paramSet.FindOneSpectrum("scale", core::color::Spectrum(1.0));
    Float fov = paramSet.FindOneFloat("fov", 45.);
    std::string texname = paramSet.FindOneFilename("mapname", "");
    return std::make_shared<ProjectionLight>(light2world, medium, I * sc,
        texname, fov);
}
*/


}
}