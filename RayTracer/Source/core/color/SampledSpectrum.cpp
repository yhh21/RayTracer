#include "SampledSpectrum.h"

namespace core
{
namespace color
{


SampledSpectrum SampledSpectrum::FromSampled(const Float *lambda, const Float *v, int n)
{
    // Sort samples if unordered, use sorted for returned spectrum
    if (!SpectrumSamplesSorted(lambda, v, n))
    {
        std::vector<Float> slambda(&lambda[0], &lambda[n]);
        std::vector<Float> sv(&v[0], &v[n]);
        SortSpectrumSamples(&slambda[0], &sv[0], n);
        return FromSampled(&slambda[0], &sv[0], n);
    }
    SampledSpectrum r;
    for (int i = 0; i < SPECTRAL_SAMPLES_NUMBER; ++i)
    {
        // Compute average value of given SPD over $i$th sample's range
        Float lambda0 = common::math::Lerp(static_cast<Float>(i) / static_cast<Float>(SPECTRAL_SAMPLES_NUMBER)
            , SAMPLED_LAMBDA_START, SAMPLED_LAMBDA_END);
        Float lambda1 = common::math::Lerp(static_cast<Float>(i + 1) / static_cast<Float>(SPECTRAL_SAMPLES_NUMBER)
            , SAMPLED_LAMBDA_START, SAMPLED_LAMBDA_END);

        r.c[i] = AverageSpectrumSamples(lambda, v, n, lambda0, lambda1);
    }
    return r;
}

void SampledSpectrum::Init()
{
    // Compute XYZ matching functions for _SampledSpectrum_
    for (int i = 0; i < SPECTRAL_SAMPLES_NUMBER; ++i)
    {
        Float wl0 = common::math::Lerp(static_cast<Float>(i) / static_cast<Float>(SPECTRAL_SAMPLES_NUMBER)
            , SAMPLED_LAMBDA_START, SAMPLED_LAMBDA_END);
        Float wl1 = common::math::Lerp(static_cast<Float>(i + 1) / static_cast<Float>(SPECTRAL_SAMPLES_NUMBER)
            , SAMPLED_LAMBDA_START, SAMPLED_LAMBDA_END);

        X.c[i] = AverageSpectrumSamples(CIE_lambda, CIE_X, CIE_SAMPLES_NUMBER, wl0, wl1);
        Y.c[i] = AverageSpectrumSamples(CIE_lambda, CIE_Y, CIE_SAMPLES_NUMBER, wl0, wl1);
        Z.c[i] = AverageSpectrumSamples(CIE_lambda, CIE_Z, CIE_SAMPLES_NUMBER, wl0, wl1);
    }

    // Compute RGB to spectrum functions for _SampledSpectrum_
    for (int i = 0; i < SPECTRAL_SAMPLES_NUMBER; ++i)
    {
        Float wl0 = common::math::Lerp(static_cast<Float>(i) / static_cast<Float>(SPECTRAL_SAMPLES_NUMBER)
            , SAMPLED_LAMBDA_START, SAMPLED_LAMBDA_END);
        Float wl1 = common::math::Lerp(static_cast<Float>(i + 1) / static_cast<Float>(SPECTRAL_SAMPLES_NUMBER)
            , SAMPLED_LAMBDA_START, SAMPLED_LAMBDA_END);

        rgbRefl2SpectWhite.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_REFL_2_SPECT_WHITE
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);
        rgbRefl2SpectCyan.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_REFL_2_SPECT_CYAN
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);
        rgbRefl2SpectMagenta.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_REFL_2_SPECT_MAGENTA
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);
        rgbRefl2SpectYellow.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_REFL_2_SPECT_YELLOW
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);
        rgbRefl2SpectRed.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_REFL_2_SPECT_RED
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);
        rgbRefl2SpectGreen.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_REFL_2_SPECT_GREEN
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);
        rgbRefl2SpectBlue.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_REFL_2_SPECT_BLUE
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);

        rgbIllum2SpectWhite.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_ILLUM_2_SPECT_WHITE
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);
        rgbIllum2SpectCyan.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_ILLUM_2_SPECT_CYAN
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);
        rgbIllum2SpectMagenta.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_ILLUM_2_SPECT_MAGENTA
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);
        rgbIllum2SpectYellow.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_ILLUM_2_SPECT_YELLOW
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);
        rgbIllum2SpectRed.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_ILLUM_2_SPECT_RED
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);
        rgbIllum2SpectGreen.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_ILLUM_2_SPECT_GREEN
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);
        rgbIllum2SpectBlue.c[i] = AverageSpectrumSamples(RGB_2_SPECT_LAMBDA, RGB_ILLUM_2_SPECT_BLUE
            , RGB_2_SPECT_SAMPLES_NUMBER, wl0, wl1);
    }
}

SampledSpectrum SampledSpectrum::FromRGB(const Float rgb[3], SpectrumType type)
{
    SampledSpectrum r;
    if (type == SpectrumType::Reflectance)
    {
        // Convert reflectance spectrum to RGB
        if (rgb[0] <= rgb[1] && rgb[0] <= rgb[2])
        {
            // Compute reflectance _SampledSpectrum_ with _rgb[0]_ as minimum
            r += rgb[0] * rgbRefl2SpectWhite;
            if (rgb[1] <= rgb[2])
            {
                r += (rgb[1] - rgb[0]) * rgbRefl2SpectCyan;
                r += (rgb[2] - rgb[1]) * rgbRefl2SpectBlue;
            }
            else
            {
                r += (rgb[2] - rgb[0]) * rgbRefl2SpectCyan;
                r += (rgb[1] - rgb[2]) * rgbRefl2SpectGreen;
            }
        }
        else if (rgb[1] <= rgb[0] && rgb[1] <= rgb[2])
        {
            // Compute reflectance _SampledSpectrum_ with _rgb[1]_ as minimum
            r += rgb[1] * rgbRefl2SpectWhite;
            if (rgb[0] <= rgb[2])
            {
                r += (rgb[0] - rgb[1]) * rgbRefl2SpectMagenta;
                r += (rgb[2] - rgb[0]) * rgbRefl2SpectBlue;
            }
            else
            {
                r += (rgb[2] - rgb[1]) * rgbRefl2SpectMagenta;
                r += (rgb[0] - rgb[2]) * rgbRefl2SpectRed;
            }
        }
        else
        {
            // Compute reflectance _SampledSpectrum_ with _rgb[2]_ as minimum
            r += rgb[2] * rgbRefl2SpectWhite;
            if (rgb[0] <= rgb[1])
            {
                r += (rgb[0] - rgb[2]) * rgbRefl2SpectYellow;
                r += (rgb[1] - rgb[0]) * rgbRefl2SpectGreen;
            }
            else
            {
                r += (rgb[1] - rgb[2]) * rgbRefl2SpectYellow;
                r += (rgb[0] - rgb[1]) * rgbRefl2SpectRed;
            }
        }

        r *= static_cast<Float>(0.94F);
    }
    else
    {
        // Convert illuminant spectrum to RGB
        if (rgb[0] <= rgb[1] && rgb[0] <= rgb[2])
        {
            // Compute illuminant _SampledSpectrum_ with _rgb[0]_ as minimum
            r += rgb[0] * rgbIllum2SpectWhite;
            if (rgb[1] <= rgb[2])
            {
                r += (rgb[1] - rgb[0]) * rgbIllum2SpectCyan;
                r += (rgb[2] - rgb[1]) * rgbIllum2SpectBlue;
            }
            else
            {
                r += (rgb[2] - rgb[0]) * rgbIllum2SpectCyan;
                r += (rgb[1] - rgb[2]) * rgbIllum2SpectGreen;
            }
        }
        else if (rgb[1] <= rgb[0] && rgb[1] <= rgb[2])
        {
            // Compute illuminant _SampledSpectrum_ with _rgb[1]_ as minimum
            r += rgb[1] * rgbIllum2SpectWhite;
            if (rgb[0] <= rgb[2])
            {
                r += (rgb[0] - rgb[1]) * rgbIllum2SpectMagenta;
                r += (rgb[2] - rgb[0]) * rgbIllum2SpectBlue;
            }
            else
            {
                r += (rgb[2] - rgb[1]) * rgbIllum2SpectMagenta;
                r += (rgb[0] - rgb[2]) * rgbIllum2SpectRed;
            }
        }
        else
        {
            // Compute illuminant _SampledSpectrum_ with _rgb[2]_ as minimum
            r += rgb[2] * rgbIllum2SpectWhite;
            if (rgb[0] <= rgb[1])
            {
                r += (rgb[0] - rgb[2]) * rgbIllum2SpectYellow;
                r += (rgb[1] - rgb[0]) * rgbIllum2SpectGreen;
            }
            else
            {
                r += (rgb[1] - rgb[2]) * rgbIllum2SpectYellow;
                r += (rgb[0] - rgb[1]) * rgbIllum2SpectRed;
            }
        }

        r *= static_cast<Float>(0.86445F);
    }

    return r.Clamp();
}

SampledSpectrum SampledSpectrum::FromXYZ(const Float xyz[3], SpectrumType type)
{
    Float rgb[3];
    XYZToRGB(xyz, rgb);
    return FromRGB(rgb, type);
}


SampledSpectrum SampledSpectrum::X;
SampledSpectrum SampledSpectrum::Y;
SampledSpectrum SampledSpectrum::Z;
SampledSpectrum SampledSpectrum::rgbRefl2SpectWhite;
SampledSpectrum SampledSpectrum::rgbRefl2SpectCyan;
SampledSpectrum SampledSpectrum::rgbRefl2SpectMagenta;
SampledSpectrum SampledSpectrum::rgbRefl2SpectYellow;
SampledSpectrum SampledSpectrum::rgbRefl2SpectRed;
SampledSpectrum SampledSpectrum::rgbRefl2SpectGreen;
SampledSpectrum SampledSpectrum::rgbRefl2SpectBlue;
SampledSpectrum SampledSpectrum::rgbIllum2SpectWhite;
SampledSpectrum SampledSpectrum::rgbIllum2SpectCyan;
SampledSpectrum SampledSpectrum::rgbIllum2SpectMagenta;
SampledSpectrum SampledSpectrum::rgbIllum2SpectYellow;
SampledSpectrum SampledSpectrum::rgbIllum2SpectRed;
SampledSpectrum SampledSpectrum::rgbIllum2SpectGreen;
SampledSpectrum SampledSpectrum::rgbIllum2SpectBlue;


}
}