#pragma once

#include "Camera.h"
#include "../../common/math/Transform.h"
#include "../../common/math/Bounds2.h"
#include "../film/Film.h"

namespace core
{
namespace camera
{


class ProjectiveCamera : public Camera
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    ProjectiveCamera(const common::math::AnimatedTransformf &CameraToWorld,
        const common::math::Transformf &cameraToScreen,
        const common::math::Bounds2f &screenWindow, Float shutterOpen,
        Float shutterClose, Float lensr, Float focald, core::film::Film *film,
        const core::interaction::Medium *medium)
        : Camera(CameraToWorld, shutterOpen, shutterClose, film, medium),
        CameraToScreen(cameraToScreen)
    {
        // Initialize depth of field parameters
        lensRadius = lensr;
        focalDistance = focald;

        // Compute projective camera transformations

        // Compute projective camera screen transformations
        ScreenToRaster = Scale(common::math::Vec3f(film->fullResolution.x, film->fullResolution.y, FLOAT_1))
            * Scale(common::math::Vec3f(FLOAT_1 / (screenWindow.point_max.x - screenWindow.point_min.x)
                , FLOAT_1 / (screenWindow.point_min.y - screenWindow.point_max.y), FLOAT_1))
            * Translate(common::math::Vec3f(-screenWindow.point_min.x, -screenWindow.point_max.y, FLOAT_0));
        RasterToScreen = Inverse(ScreenToRaster);
        RasterToCamera = Inverse(CameraToScreen) * RasterToScreen;
    }

protected:

    common::math::Transformf CameraToScreen, RasterToCamera;

    common::math::Transformf ScreenToRaster, RasterToScreen;

    Float lensRadius, focalDistance;
};


}
}