#pragma once

#include "../../ForwardDeclaration.h"
#include "../interaction/Medium.h"
#include "../../common/math/Transform.h"

namespace core
{
namespace light
{


enum class LightFlags : int
{
    DeltaPosition = 1,
    DeltaDirection = 2,
    Area = 4,
    Infinite = 8
};

inline
bool IsDeltaLight(int flags)
{
    return (flags & (int)LightFlags::DeltaPosition) || (flags & (int)LightFlags::DeltaDirection);
}


class Light
{
public:

    const int flags;
    const int nSamples;
    const interaction::MediumInterface mediumInterface;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    Light(int flags, const common::math::Transformf &LightToWorld,
        const interaction::MediumInterface &mediumInterface, int nSamples = 1);

    virtual ~Light();


    virtual color::Spectrum Sample_Li(const interaction::Interaction &ref, const common::math::Vec2f &u,
        common::math::Vec3f *wi, Float *pdf,
        VisibilityTester *vis) const = 0;

    virtual color::Spectrum Power() const = 0;

    virtual void Preprocess(const core::scene::Scene &scene)
    {}


    virtual color::Spectrum Le(const common::math::RayDifferentialf &r) const;

    virtual Float Pdf_Li(const interaction::Interaction &ref, const common::math::Vec3f &wi) const = 0;

    virtual color::Spectrum Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2, Float time,
        common::math::Rayf *ray, common::math::Vec3f *nLight, Float *pdfPos,
        Float *pdfDir) const = 0;

    virtual void Pdf_Le(const common::math::Rayf &ray, const common::math::Vec3f &nLight, Float *pdfPos,
        Float *pdfDir) const = 0;

protected:

    const common::math::Transformf LightToWorld, WorldToLight;
};


}
}