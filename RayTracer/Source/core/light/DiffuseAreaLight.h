#pragma once

#include "AreaLight.h"
#include "../../common/math/Vec3.h"
#include "../color/Spectrum.h"
#include "../interaction/Interaction.h"

namespace core
{
namespace light
{


class DiffuseAreaLight : public AreaLight
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    DiffuseAreaLight(const common::math::Transformf &LightToWorld,
        const interaction::MediumInterface &medium_interface, const core::color::Spectrum &Le,
        int nSamples, const std::shared_ptr<core::shape::Shape> &shape,
        bool twoSided = false);


    core::color::Spectrum L(const interaction::Interaction &intr, const common::math::Vec3f &w) const
    {
        return (twoSided || Dot(intr.n, w) > FLOAT_0) ? Lemit : core::color::Spectrum(FLOAT_0);
    }

    core::color::Spectrum Power() const;

    core::color::Spectrum Sample_Li(const interaction::Interaction &ref, const common::math::Vec2f &u, common::math::Vec3f *wo,
        Float *pdf, VisibilityTester *vis) const;

    Float Pdf_Li(const interaction::Interaction &, const common::math::Vec3f &) const;

    core::color::Spectrum Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2, Float time,
        common::math::Rayf *ray, common::math::Vec3f *nLight, Float *pdfPos,
        Float *pdfDir) const;

    void Pdf_Le(const common::math::Rayf &, const common::math::Vec3f &, Float *pdfPos,
        Float *pdfDir) const;

protected:

    const core::color::Spectrum Lemit;

    std::shared_ptr<core::shape::Shape> shape;
    // Added after book publication: by default, DiffuseAreaLights still
    // only emit in the hemimsphere around the surface normal.  However,
    // this behavior can now be overridden to give emission on both sides.
    const bool twoSided;

    const Float area;
};

/* TODO
std::shared_ptr<AreaLight> CreateDiffuseAreaLight(
    const common::math::Transformf &light2world, const Medium *medium,
    const ParamSet &paramSet, const std::shared_ptr<core::shape::Shape> &shape);
*/


}
}