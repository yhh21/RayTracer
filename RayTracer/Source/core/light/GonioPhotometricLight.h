#pragma once

#include "Light.h"
#include "../../common/math/Vec3.h"
#include "../../common/tool/MIPMap.h"
#include "../color/Spectrum.h"

namespace core
{
namespace light
{


class GonioPhotometricLight : public Light
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    GonioPhotometricLight(const common::math::Transformf &LightToWorld,
        const interaction::MediumInterface &medium_interface,
        const core::color::Spectrum &I, const std::string &texname)
        : Light(static_cast<int>(LightFlags::DeltaPosition), LightToWorld, medium_interface),
        pLight(LightToWorld(common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0))),
        I(I)
    {
        // Create _mipmap_ for _GonioPhotometricLight_
        common::math::Vec2i resolution;
        /* TODO
        std::unique_ptr<core::color::RGBSpectrum[]> texels = ReadImage(texname, &resolution);
        if (texels)
        {
            mipmap.reset(new MIPMap<core::color::RGBSpectrum>(resolution, texels.get()));
        }
        */
    }


    core::color::Spectrum Sample_Li(const interaction::Interaction &ref, const common::math::Vec2f &u, common::math::Vec3f *wi,
        Float *pdf, VisibilityTester *vis) const;

    core::color::Spectrum Scale(const common::math::Vec3f &w) const
    {
        common::math::Vec3f wp = Normalize(WorldToLight(w));
        std::swap(wp.y, wp.z);
        Float theta = SphericalTheta(wp);
        Float phi = SphericalPhi(wp);
        common::math::Vec2f st(phi * common::math::INV_TWO_PI, theta * common::math::INV_PI);
        return !mipmap ? core::color::RGBSpectrum(FLOAT_1)
            : core::color::Spectrum(mipmap->Lookup(st), core::color::SpectrumType::Illuminant);
    }

    core::color::Spectrum Power() const;

    Float Pdf_Li(const interaction::Interaction &, const common::math::Vec3f &) const;

    core::color::Spectrum Sample_Le(const common::math::Vec2f &u1, const common::math::Vec2f &u2, Float time,
        common::math::Rayf *ray, common::math::Vec3f *nLight, Float *pdfPos,
        Float *pdfDir) const;

    void Pdf_Le(const common::math::Rayf &, const common::math::Vec3f &, Float *pdfPos,
        Float *pdfDir) const;

private:

    const common::math::Vec3f pLight;

    const core::color::Spectrum I;

    std::unique_ptr<common::tool::MIPMap<core::color::RGBSpectrum>> mipmap;
};

/* TODO
std::shared_ptr<GonioPhotometricLight> CreateGoniometricLight(
    const common::math::Transformf &light2world, const Medium *medium,
    const ParamSet &paramSet);
*/


}
}