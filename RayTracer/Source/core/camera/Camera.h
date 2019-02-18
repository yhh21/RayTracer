#pragma once

#include "../../ForwardDeclaration.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Vec3.h"
#include "../../common/math/AnimatedTransform.h"

namespace core
{
namespace camera
{


struct CameraSample
{
    common::math::Vec2f pFilm;
    common::math::Vec2f pLens;
    Float time;
};

class Camera
{
public:

    common::math::AnimatedTransformf CameraToWorld;
    const Float shutterOpen, shutterClose;
    core::film::Film *film;
    const interaction::Medium *medium;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    Camera(const common::math::AnimatedTransformf &CameraToWorld, Float shutterOpen,
        Float shutterClose, core::film::Film *film, const interaction::Medium *medium);

    virtual ~Camera();


    virtual Float GenerateRay(const CameraSample &sample, common::math::Rayf *ray) const = 0;

    virtual Float GenerateRayDifferential(const CameraSample &sample, common::math::RayDifferentialf *rd) const;

    virtual color::Spectrum We(const common::math::Rayf &ray, common::math::Vec2f *pRaster2 = nullptr) const;

    virtual void Pdf_We(const common::math::Rayf &ray, Float *pdfPos, Float *pdfDir) const;

    virtual color::Spectrum Sample_Wi(const interaction::Interaction &ref, const common::math::Vec2f &u
        , common::math::Vec3f *wi, Float *pdf
        , common::math::Vec2f *pRaster
        , core::light::VisibilityTester *vis) const;

};


}
}