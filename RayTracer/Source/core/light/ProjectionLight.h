#pragma once

#include "Light.h"
#include "../../common/math/Vec3.h"
#include "../../common/math/Bounds2.h"
#include "../../common/tool/MIPMap.h"
#include "../color/Spectrum.h"
#include "../color/RGBSpectrum.h"

namespace core
{
namespace light
{


class ProjectionLight : public Light
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    ProjectionLight(const common::math::Transformf &LightToWorld,
        const interaction::MediumInterface &medium, const core::color::Spectrum &I,
        const std::string &texname, Float fov);


    core::color::Spectrum Sample_Li(const interaction::Interaction &ref, const common::math::Vec2f &u, common::math::Vec3f *wi,
        Float *pdf, VisibilityTester *vis) const;

    core::color::Spectrum Projection(const common::math::Vec3f &w) const;

    core::color::Spectrum Power() const;

    Float Pdf_Li(const interaction::Interaction &, const common::math::Vec3f &) const;

    core::color::Spectrum Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2, Float time,
        common::math::Rayf *ray, common::math::Vec3f *nLight, Float *pdfPos,
        Float *pdfDir) const;

    void Pdf_Le(const common::math::Rayf &, const common::math::Vec3f &, Float *pdfPos,
        Float *pdfDir) const;

private:

    std::unique_ptr<common::tool::MIPMap<core::color::RGBSpectrum>> projectionMap;

    const common::math::Vec3f pLight;

    const core::color::Spectrum I;

    common::math::Transformf lightProjection;

    Float hither, yon;

    common::math::Bounds2f screenBounds;

    Float cosTotalWidth;
};

/* TODO
std::shared_ptr<ProjectionLight> CreateProjectionLight(
    const common::math::Transformf &light2world, const Medium *medium,
    const ParamSet &paramSet);
*/


}
}