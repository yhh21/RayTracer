#include "EnvironmentCamera.h"

#include "../film/Film.h"
#include "../interaction/Interaction.h"
#include "../sampler/Sampling.h"
#include "../../common/math/Constants.h"
#include "../../common/math/Ray.h"
#include "../../common/math/RayDifferential.h"

namespace core
{
namespace camera
{


Float EnvironmentCamera::GenerateRay(const CameraSample &sample,
    common::math::Rayf *ray) const
{
    //ProfilePhase prof(Prof::GenerateCameraRay);
    // Compute environment camera ray direction
    Float theta = common::math::PI * sample.pFilm.y / film->fullResolution.y;
    Float phi = FLOAT_2 * common::math::PI * sample.pFilm.x / film->fullResolution.x;
    common::math::Vec3f dir(std::sin(theta) * std::cos(phi), std::cos(theta),
        std::sin(theta) * std::sin(phi));
    *ray = common::math::Rayf(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0), dir
        , (std::numeric_limits<Float>::max)(), FLOAT_0,
        common::math::Lerp(sample.time, shutterOpen, shutterClose));
    ray->medium = medium;
    *ray = CameraToWorld(*ray);
    return FLOAT_1;
}

/* TODO
EnvironmentCamera *CreateEnvironmentCamera(const ParamSet &params,
    const common::math::AnimatedTransformf &cam2world,
    core::film::Film *film, const core::interaction::Medium *medium)
{
    // Extract common camera parameters from _ParamSet_
    Float shutteropen = params.FindOneFloat("shutteropen", 0.f);
    Float shutterclose = params.FindOneFloat("shutterclose", 1.f);
    if (shutterclose < shutteropen)
    {
        Warning("Shutter close time [%f] < shutter open [%f].  Swapping them.",
            shutterclose, shutteropen);
        std::swap(shutterclose, shutteropen);
    }
    Float lensradius = params.FindOneFloat("lensradius", 0.f);
    Float focaldistance = params.FindOneFloat("focaldistance", 1e30f);
    Float frame = params.FindOneFloat(
        "frameaspectratio",
        Float(film->fullResolution.x) / Float(film->fullResolution.y));
    common::math::Bounds2f screen;
    if (frame > 1.f)
    {
        screen.pMin.x = -frame;
        screen.pMax.x = frame;
        screen.pMin.y = -1.f;
        screen.pMax.y = 1.f;
    }
    else
    {
        screen.pMin.x = -1.f;
        screen.pMax.x = 1.f;
        screen.pMin.y = -1.f / frame;
        screen.pMax.y = 1.f / frame;
    }
    int swi;
    const Float *sw = params.FindFloat("screenwindow", &swi);
    if (sw)
    {
        if (4 == swi)
        {
            screen.pMin.x = sw[0];
            screen.pMax.x = sw[1];
            screen.pMin.y = sw[2];
            screen.pMax.y = sw[3];
        }
        else
            Error("\"screenwindow\" should have four values");
    }
    (void)lensradius;     // don't need this
    (void)focaldistance;  // don't need this
    return new EnvironmentCamera(cam2world, shutteropen, shutterclose, film,
        medium);
}
*/


}
}