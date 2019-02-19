#pragma once

#include "../../ForwardDeclaration.h"
#include "Light.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Vec3.h"
#include "../color/Spectrum.h"
#include "../interaction/MediumInteraction.h"

namespace core
{
namespace light
{


class PointLight : public Light
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    PointLight(const common::math::Transformf &LightToWorld,
        const core::interaction::MediumInterface &mediumInterface, const color::Spectrum &I)
        : Light((int)LightFlags::DeltaPosition, LightToWorld, mediumInterface),
        pLight(LightToWorld(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0))),
        I(I)
    {}


    color::Spectrum Sample_Li(const core::interaction::Interaction &ref, const common::math::Vec2f &u, common::math::Vec3f *wi,
        Float *pdf, VisibilityTester *vis) const;

    color::Spectrum Power() const;

    Float Pdf_Li(const core::interaction::Interaction &, const common::math::Vec3f &) const;

    color::Spectrum Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2, Float time,
        common::math::Rayf *ray, common::math::Vec3f *nLight, Float *pdfPos,
        Float *pdfDir) const;

    void Pdf_Le(const common::math::Rayf &, const common::math::Vec3f &, Float *pdfPos,
        Float *pdfDir) const;

private:

    const common::math::Vec3f pLight;

    const color::Spectrum I;
};

/* TODO
std::shared_ptr<PointLight> CreatePointLight(const common::math::Transformf &light2world,
    const Medium *medium,
    const ParamSet &paramSet);
*/

}
}