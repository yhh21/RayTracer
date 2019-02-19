#pragma once

#include "ProjectiveCamera.h"

namespace core
{
namespace camera
{


class OrthographicCamera : public ProjectiveCamera
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    OrthographicCamera(const common::math::AnimatedTransformf &CameraToWorld,
        const common::math::Bounds2f &screenWindow, Float shutterOpen,
        Float shutterClose, Float lensRadius,
        Float focalDistance, core::film::Film *film, const core::interaction::Medium *medium)
        : ProjectiveCamera(CameraToWorld, common::math::Orthographic(FLOAT_0, FLOAT_1), screenWindow,
            shutterOpen, shutterClose, lensRadius, focalDistance,
            film, medium)
    {
        // Compute differential changes in origin for orthographic camera rays
        dxCamera = RasterToCamera(common::math::Vec3f(FLOAT_1, FLOAT_0, FLOAT_0));
        dyCamera = RasterToCamera(common::math::Vec3f(FLOAT_0, FLOAT_1, FLOAT_0));
    }


    Float GenerateRay(const CameraSample &sample, common::math::Rayf *) const;

    Float GenerateRayDifferential(const CameraSample &sample,
        common::math::RayDifferentialf *) const;

private:
    // OrthographicCamera Private Data
    common::math::Vec3f dxCamera, dyCamera;
};

/* TODO
OrthographicCamera *CreateOrthographicCamera(const ParamSet &params,
    const AnimatedTransform &cam2world,
    Film *film, const Medium *medium);

}
*/


}
}