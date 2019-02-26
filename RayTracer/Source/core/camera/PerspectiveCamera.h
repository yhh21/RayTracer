#pragma once

#include "ProjectiveCamera.h"

namespace core
{
namespace camera
{


class PerspectiveCamera : public ProjectiveCamera
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    PerspectiveCamera(const common::math::AnimatedTransformf &CameraToWorld,
        const common::math::Bounds2f &screenWindow, Float shutterOpen,
        Float shutterClose, Float lensRadius, Float focalDistance,
        Float fov, core::film::Film *film, const core::interaction::Medium *medium);


    Float GenerateRay(const CameraSample &sample, common::math::Rayf *) const;

    Float GenerateRayDifferential(const CameraSample &sample,
        common::math::RayDifferentialf *ray) const;

    core::color::Spectrum We(const common::math::Rayf &ray, common::math::Vec2f *pRaster2 = nullptr) const;

    void Pdf_We(const common::math::Rayf &ray, Float *pdfPos, Float *pdfDir) const;

    core::color::Spectrum Sample_Wi(const core::interaction::Interaction &ref, const common::math::Vec2f &sample,
        common::math::Vec3f *wi, Float *pdf, common::math::Vec2f *pRaster,
        core::light::VisibilityTester *vis) const;

private:

    common::math::Vec3f dxCamera, dyCamera;

    Float A;
};

/* TODO
PerspectiveCamera *CreatePerspectiveCamera(const ParamSet &params,
    const common::math::AnimatedTransformf &cam2world,
    core::film::Film *film, const core::interaction::Medium *medium);
*/


}
}