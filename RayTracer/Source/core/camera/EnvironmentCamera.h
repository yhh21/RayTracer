#pragma once

#include "Camera.h"

namespace core
{
namespace camera
{


class EnvironmentCamera : public Camera
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    EnvironmentCamera(const common::math::AnimatedTransformf &CameraToWorld, Float shutterOpen,
        Float shutterClose, core::film::Film *film, const core::interaction::Medium *medium)
        : Camera(CameraToWorld, shutterOpen, shutterClose, film, medium)
    {}

    Float GenerateRay(const CameraSample &sample, common::math::Rayf *) const;
};

/* TODO
EnvironmentCamera *CreateEnvironmentCamera(const ParamSet &params,
    const common::math::AnimatedTransformf &cam2world,
    core::film::Film *film, const core::interaction::Medium *medium);
*/


}
}