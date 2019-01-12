#include "Camera.h"

#include "../film/Film.h"
#include "../interaction/Interaction.h"

namespace core
{
namespace camera
{

Camera::Camera(const common::math::AnimatedTransformf &CameraToWorld, Float shutterOpen,
    Float shutterClose, film::Film *film, const Medium *medium)
    : CameraToWorld(CameraToWorld),
    shutterOpen(shutterOpen),
    shutterClose(shutterClose),
    film(film),
    medium(medium)
{
    /// TODO
    /*
    if (CameraToWorld.HasScale())
        Warning(
            "Scaling detected in world-to-camera transformation!\n"
            "The system has numerous assumptions, implicit and explicit,\n"
            "that this transform will have no scale factors in it.\n"
            "Proceed at your own risk; your image may have errors or\n"
            "the system may crash as a result of this.");
            */
}

Camera::~Camera()
{
    delete film;
}


Float Camera::GenerateRayDifferential(const CameraSample &sample, common::math::RayDifferentialf *rd) const
{
    Float wt = GenerateRay(sample, rd);
    if (FLOAT_0 == wt) return FLOAT_0;

    Float FLOAT_0_05 = static_cast<Float>(0.05F);
    /// Find camera ray after shifting a fraction of a pixel in the $x$ direction
    Float wtx;
    for (Float eps : { FLOAT_0_05, -FLOAT_0_05 })
    {
        CameraSample sshift = sample;
        sshift.pFilm.x += eps;
        common::math::Rayf rx;
        wtx = GenerateRay(sshift, &rx);
        rd->rx_origin = rd->origin + (rx.origin - rd->origin) / eps;
        rd->rx_direction = rd->dir + (rx.dir - rd->dir) / eps;
        
        if (FLOAT_0 != wtx) break;
    }
    if (FLOAT_0 == wtx) return FLOAT_0;

    /// Find camera ray after shifting a fraction of a pixel in the $y$ direction
    Float wty;
    for (Float eps : { FLOAT_0_05, FLOAT_0_05 })
    {
        CameraSample sshift = sample;
        sshift.pFilm.y += eps;
        common::math::Rayf ry;
        wty = GenerateRay(sshift, &ry);
        rd->rx_origin = rd->origin + (ry.origin - rd->origin) / eps;
        rd->rx_direction = rd->dir + (ry.dir - rd->dir) / eps;
        
        if (FLOAT_0 != wty) break;
    }
    if (FLOAT_0 == wty) return FLOAT_0;

    rd->has_differentials = true;
    return wt;
}

color::Spectrum Camera::We(const common::math::Rayf &ray, common::math::Vec2f *raster) const
{
    //LOG(FATAL) << "Camera::We() is not implemented!";
    return color::Spectrum(FLOAT_0);
}

void Camera::Pdf_We(const common::math::Rayf &ray, Float *pdfPos, Float *pdfDir) const
{
    //LOG(FATAL) << "Camera::Pdf_We() is not implemented!";
}

color::Spectrum Camera::Sample_Wi(const interaction::Interaction &ref, const common::math::Vec2f &u
    , common::math::Vec3f *wi, Float *pdf, common::math::Vec2f *pRaster
    , VisibilityTester *vis) const
{
    //LOG(FATAL) << "Camera::Sample_Wi() is not implemented!";
    return Spectrum(FLOAT_0);
}


}
}