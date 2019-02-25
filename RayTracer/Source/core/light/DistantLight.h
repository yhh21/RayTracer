#pragma once

#include "Light.h"
#include "../../common/math/Vec3.h"
#include "../color/Spectrum.h"
#include "../scene/Scene.h"

namespace core
{
namespace light
{


class DistantLight : public Light
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    DistantLight(const common::math::Transformf &LightToWorld, const core::color::Spectrum &L,
        const common::math::Vec3f &w);


    void Preprocess(const core::scene::Scene &scene)
    {
        scene.WorldBound().BoundingSphere(&worldCenter, &worldRadius);
    }

    core::color::Spectrum Sample_Li(const interaction::Interaction &ref, const common::math::Vec2f &u, common::math::Vec3f *wi,
        Float *pdf, VisibilityTester *vis) const;

    core::color::Spectrum Power() const;

    Float Pdf_Li(const interaction::Interaction &, const common::math::Vec3f &) const;

    core::color::Spectrum Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2, Float time,
        common::math::Rayf *ray, common::math::Vec3f *nLight, Float *pdfPos,
        Float *pdfDir) const;

    void Pdf_Le(const common::math::Rayf &, const common::math::Vec3f &, Float *pdfPos,
        Float *pdfDir) const;

private:

    const core::color::Spectrum L;

    const common::math::Vec3f wLight;

    common::math::Vec3f worldCenter;

    Float worldRadius;
};

/* TODO
std::shared_ptr<DistantLight> CreateDistantLight(const common::math::Transformf &light2world,
    const ParamSet &paramSet);
*/


}
}