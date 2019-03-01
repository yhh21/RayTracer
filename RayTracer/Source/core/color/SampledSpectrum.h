#pragma once

#include "CoefficientSpectrum.h"
#include "RGBSpectrum.h"

namespace core
{
namespace color
{


class SampledSpectrum : public CoefficientSpectrum<SPECTRAL_SAMPLES_NUMBER>
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    SampledSpectrum(Float v = FLOAT_0) : CoefficientSpectrum<SPECTRAL_SAMPLES_NUMBER>(v)
    {}

    SampledSpectrum(CoefficientSpectrum<SPECTRAL_SAMPLES_NUMBER> v) : CoefficientSpectrum<SPECTRAL_SAMPLES_NUMBER>(v)
    {}

    SampledSpectrum(const RGBSpectrum &r, SpectrumType type = SpectrumType::Reflectance)
    {
        Float rgb[3];
        r.ToRGB(rgb);
        *this = SampledSpectrum::FromRGB(rgb, type);
    }


    static SampledSpectrum FromSampled(const Float *lambda, const Float *v, int n);

    static void Init();


    Float y() const
    {
        Float yy = FLOAT_0;
        for (int i = 0; i < SPECTRAL_SAMPLES_NUMBER; ++i)
        {
            yy += Y.c[i] * c[i];
        }

        return yy * static_cast<Float>(SAMPLED_LAMBDA_END - SAMPLED_LAMBDA_START)
            / static_cast<Float>(CIE_Y_INTEGRAL * SPECTRAL_SAMPLES_NUMBER);
    }

    void ToXYZ(Float xyz[3]) const
    {
        xyz[0] = xyz[1] = xyz[2] = FLOAT_0;
        for (int i = 0; i < SPECTRAL_SAMPLES_NUMBER; ++i)
        {
            xyz[0] += X.c[i] * c[i];
            xyz[1] += Y.c[i] * c[i];
            xyz[2] += Z.c[i] * c[i];
        }

        Float scale = static_cast<Float>(SAMPLED_LAMBDA_END - SAMPLED_LAMBDA_START)
            / static_cast<Float>(CIE_Y_INTEGRAL * SPECTRAL_SAMPLES_NUMBER);

        xyz[0] *= scale;
        xyz[1] *= scale;
        xyz[2] *= scale;
    }

    void ToRGB(Float rgb[3]) const
    {
        Float xyz[3];
        ToXYZ(xyz);
        XYZToRGB(xyz, rgb);
    }

    RGBSpectrum ToRGBSpectrum() const
    {
        Float rgb[3];
        ToRGB(rgb);
        return RGBSpectrum::FromRGB(rgb);
    }

    static SampledSpectrum FromRGB(
        const Float rgb[3], SpectrumType type = SpectrumType::Illuminant);

    static SampledSpectrum FromXYZ(
        const Float xyz[3], SpectrumType type = SpectrumType::Reflectance);

private:
    static SampledSpectrum X, Y, Z;
    static SampledSpectrum rgbRefl2SpectWhite, rgbRefl2SpectCyan;
    static SampledSpectrum rgbRefl2SpectMagenta, rgbRefl2SpectYellow;
    static SampledSpectrum rgbRefl2SpectRed, rgbRefl2SpectGreen;
    static SampledSpectrum rgbRefl2SpectBlue;
    static SampledSpectrum rgbIllum2SpectWhite, rgbIllum2SpectCyan;
    static SampledSpectrum rgbIllum2SpectMagenta, rgbIllum2SpectYellow;
    static SampledSpectrum rgbIllum2SpectRed, rgbIllum2SpectGreen;
    static SampledSpectrum rgbIllum2SpectBlue;
};


}
}