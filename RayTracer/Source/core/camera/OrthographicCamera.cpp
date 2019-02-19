#include "OrthographicCamera.h"

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


Float OrthographicCamera::GenerateRay(const CameraSample &sample,
    common::math::Rayf *ray) const
{
    //ProfilePhase prof(Prof::GenerateCameraRay);
    // Compute raster and camera sample positions
    common::math::Vec3f pFilm = common::math::Vec3f(sample.pFilm.x, sample.pFilm.y, FLOAT_0);
    common::math::Vec3f pCamera = RasterToCamera(pFilm);
    *ray = common::math::Rayf(pCamera, common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_1));
    // Modify ray for depth of field
    if (lensRadius > FLOAT_0)
    {
        // Sample point on lens
        common::math::Vec2f pLens = lensRadius * core::sampler::ConcentricSampleDisk(sample.pLens);

        // Compute point on plane of focus
        Float ft = focalDistance / ray->dir.z;
        common::math::Vec3f pFocus = (*ray)(ft);

        // Update ray for effect of lens
        ray->origin = common::math::Vec3f(pLens.x, pLens.y, FLOAT_0);
        ray->dir = Normalize(pFocus - ray->origin);
    }
    ray->time = common::math::Lerp(sample.time, shutterOpen, shutterClose);
    ray->medium = medium;
    *ray = CameraToWorld(*ray);

    return FLOAT_1;
}

Float OrthographicCamera::GenerateRayDifferential(const CameraSample &sample,
    common::math::RayDifferentialf *ray) const
{
    //ProfilePhase prof(Prof::GenerateCameraRay);
    // Compute main orthographic viewing ray

    // Compute raster and camera sample positions
    common::math::Vec3f pFilm = common::math::Vec3f(sample.pFilm.x, sample.pFilm.y, FLOAT_0);
    common::math::Vec3f pCamera = RasterToCamera(pFilm);
    *ray = common::math::RayDifferentialf(pCamera, common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_1));

    // Modify ray for depth of field
    if (lensRadius > FLOAT_0)
    {
        // Sample point on lens
        common::math::Vec2f pLens = lensRadius * core::sampler::ConcentricSampleDisk(sample.pLens);

        // Compute point on plane of focus
        Float ft = focalDistance / ray->dir.z;
        common::math::Vec3f pFocus = (*ray)(ft);

        // Update ray for effect of lens
        ray->origin = common::math::Vec3f(pLens.x, pLens.y, FLOAT_0);
        ray->dir = Normalize(pFocus - ray->origin);
    }

    // Compute ray differentials for _OrthographicCamera_
    if (lensRadius > FLOAT_0)
    {
        // Compute _OrthographicCamera_ ray differentials accounting for lens

        // Sample point on lens
        common::math::Vec2f pLens = lensRadius * core::sampler::ConcentricSampleDisk(sample.pLens);
        Float ft = focalDistance / ray->dir.z;

        common::math::Vec3f pFocus = pCamera + dxCamera + (ft * common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_1));
        ray->rx_origin = common::math::Vec3f(pLens.x, pLens.y, FLOAT_0);
        ray->rx_direction = Normalize(pFocus - ray->rx_origin);

        pFocus = pCamera + dyCamera + (ft * common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_1));
        ray->ry_origin = common::math::Vec3f(pLens.x, pLens.y, FLOAT_0);
        ray->ry_direction = Normalize(pFocus - ray->ry_origin);
    }
    else
    {
        ray->rx_origin = ray->origin + dxCamera;
        ray->ry_origin = ray->origin + dyCamera;
        ray->rx_direction = ray->ry_direction = ray->dir;
    }
    ray->time = common::math::Lerp(sample.time, shutterOpen, shutterClose);
    ray->has_differentials = true;
    ray->medium = medium;
    *ray = CameraToWorld(*ray);

    return FLOAT_1;
}

/* TODO
OrthographicCamera *CreateOrthographicCamera(const ParamSet &params,
    const AnimatedTransform &cam2world,
    Film *film, const Medium *medium)
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
    Float focaldistance = params.FindOneFloat("focaldistance", 1e6f);
    Float frame = params.FindOneFloat(
        "frameaspectratio",
        Float(film->fullResolution.x) / Float(film->fullResolution.y));
    Bounds2f screen;
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
        if (swi == 4)
        {
            screen.pMin.x = sw[0];
            screen.pMax.x = sw[1];
            screen.pMin.y = sw[2];
            screen.pMax.y = sw[3];
        }
        else
            Error("\"screenwindow\" should have four values");
    }
    return new OrthographicCamera(cam2world, screen, shutteropen, shutterclose,
        lensradius, focaldistance, film, medium);
}
*/


}
}