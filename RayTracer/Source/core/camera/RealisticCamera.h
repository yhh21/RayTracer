#pragma once

#include "Camera.h"

namespace core
{
namespace camera
{


class RealisticCamera : public Camera
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    RealisticCamera(const common::math::AnimatedTransformf &CameraToWorld, Float shutterOpen,
        Float shutterClose, Float apertureDiameter,
        Float focusDistance, bool simpleWeighting,
        std::vector<Float> &lensData, core::film::Film *film,
        const core::interaction::Medium *medium);

    Float GenerateRay(const CameraSample &sample, common::math::Rayf *) const;

private:

    struct LensElementInterface
    {
        Float curvatureRadius;
        Float thickness;
        Float eta;
        Float apertureRadius;
    };

    const bool simpleWeighting;

    std::vector<LensElementInterface> elementInterfaces;

    std::vector<common::math::Bounds2f> exitPupilBounds;


    Float LensRearZ() const
    {
        return elementInterfaces.back().thickness;
    }

    Float LensFrontZ() const
    {
        Float zSum = 0;
        for (const LensElementInterface &element : elementInterfaces)
            zSum += element.thickness;
        return zSum;
    }

    Float RearElementRadius() const
    {
        return elementInterfaces.back().apertureRadius;
    }

    bool TraceLensesFromFilm(const common::math::Rayf &ray, common::math::Rayf *rOut) const;

    static bool IntersectSphericalElement(Float radius, Float zCenter,
        const common::math::Rayf &ray, Float *t,
        common::math::Vec3f *n);

    bool TraceLensesFromScene(const common::math::Rayf &rCamera, common::math::Rayf *rOut) const;

    /* TODO
    void DrawLensSystem() const;

    void DrawRayPathFromFilm(const common::math::Rayf &r, bool arrow,
        bool toOpticalIntercept) const;

    void DrawRayPathFromScene(const common::math::Rayf &r, bool arrow,
        bool toOpticalIntercept) const;
    */

    static void ComputeCardinalPoints(const common::math::Rayf &rIn, const common::math::Rayf &rOut, Float *p,
        Float *f);

    void ComputeThickLensApproximation(Float pz[2], Float f[2]) const;

    Float FocusThickLens(Float focusDistance);

    Float FocusBinarySearch(Float focusDistance);

    Float FocusDistance(Float filmDist);

    common::math::Bounds2f BoundExitPupil(Float pFilmX0, Float pFilmX1) const;

    void RenderExitPupil(Float sx, Float sy, const char *filename) const;

    common::math::Vec3f SampleExitPupil(const common::math::Vec2f &pFilm, const common::math::Vec2f &lensSample,
        Float *sampleBoundsArea) const;

    /* TODO
    void TestExitPupilBounds() const;
    */
};

/* TODO
RealisticCamera *CreateRealisticCamera(const ParamSet &params,
    const common::math::AnimatedTransformf &cam2world,
    core::film::Film *film, const core::interaction::Medium *medium);
*/


}
}