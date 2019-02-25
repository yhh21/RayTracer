#pragma once

#include "AreaLight.h"
#include "../../common/math/Vec3.h"
#include "../../common/tool/MIPMap.h"
#include "../color/Spectrum.h"
#include "../scene/Scene.h"

namespace core
{
namespace light
{


class InfiniteAreaLight : public Light
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    InfiniteAreaLight(const common::math::Transformf &LightToWorld, const core::color::Spectrum &power,
        int nSamples, const std::string &texmap);


    void Preprocess(const core::scene::Scene &scene)
    {
        scene.WorldBound().BoundingSphere(&worldCenter, &worldRadius);
    }

    core::color::Spectrum Power() const;

    core::color::Spectrum Le(const common::math::RayDifferentialf &ray) const;

    core::color::Spectrum Sample_Li(const interaction::Interaction &ref, const common::math::Vec2f &u, common::math::Vec3f *wi,
        Float *pdf, VisibilityTester *vis) const;

    Float Pdf_Li(const interaction::Interaction &, const common::math::Vec3f &) const;

    core::color::Spectrum Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2, Float time,
        common::math::Rayf *ray, common::math::Vec3f *nLight, Float *pdfPos,
        Float *pdfDir) const;

    void Pdf_Le(const common::math::Rayf &, const common::math::Vec3f &, Float *pdfPos,
        Float *pdfDir) const;

private:

    std::unique_ptr<common::tool::MIPMap<core::color::RGBSpectrum>> Lmap;

    common::math::Vec3f worldCenter;

    Float worldRadius;

    std::unique_ptr<sampler::Distribution2D> distribution;
};

/* TODO
std::shared_ptr<InfiniteAreaLight> CreateInfiniteLight(
    const common::math::Transformf &light2world, const ParamSet &paramSet);
*/


}
}