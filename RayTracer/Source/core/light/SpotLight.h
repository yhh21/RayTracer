#pragma once

#include "Light.h"
#include "../../common/math/Vec3.h"
#include "../color/Spectrum.h"

namespace core
{
namespace light
{


class SpotLight : public Light
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    SpotLight(const common::math::Transformf &LightToWorld, const core::interaction::MediumInterface &m,
        const core::color::Spectrum &I, Float totalWidth, Float falloffStart);


    core::color::Spectrum Sample_Li(const interaction::Interaction &ref, const common::math::Vec2f &u
        , common::math::Vec3f *wi,
        Float *pdf, VisibilityTester *vis) const;

    Float Falloff(const common::math::Vec3f &w) const;

    core::color::Spectrum Power() const;

    Float Pdf_Li(const interaction::Interaction &, const common::math::Vec3f &) const;

    core::color::Spectrum Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2, Float time,
        common::math::Rayf *ray, common::math::Vec3f *nLight, Float *pdfPos,
        Float *pdfDir) const;

    void Pdf_Le(const common::math::Rayf &, const common::math::Vec3f &, Float *pdfPos,
        Float *pdfDir) const;

private:

    const common::math::Vec3f pLight;

    const core::color::Spectrum I;

    const Float cosTotalWidth, cosFalloffStart;
};

/* TODO
std::shared_ptr<SpotLight> CreateSpotLight(const common::math::Transformf &l2w,
    const Medium *medium,
    const ParamSet &paramSet);

*/


}
}