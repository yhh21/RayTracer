#include "RGBSpectrum.h"

namespace core
{
namespace color
{

RGBSpectrum RGBSpectrum::FromRGB(const Float rgb[3], SpectrumType type)
{
    RGBSpectrum s;
    s.c[0] = rgb[0];
    s.c[1] = rgb[1];
    s.c[2] = rgb[2];
    return s;
}


RGBSpectrum RGBSpectrum::FromXYZ(const Float xyz[3], SpectrumType type)
{
    RGBSpectrum r;
    XYZToRGB(xyz, r.c);
    return r;
}

RGBSpectrum RGBSpectrum::FromSampled(const Float *lambda, const Float *v, int n)
{
    // Sort samples if unordered, use sorted for returned spectrum
    if (!SpectrumSamplesSorted(lambda, v, n))
    {
        std::vector<Float> slambda(&lambda[0], &lambda[n]);
        std::vector<Float> sv(&v[0], &v[n]);
        SortSpectrumSamples(&slambda[0], &sv[0], n);
        return FromSampled(&slambda[0], &sv[0], n);
    }
    Float xyz[3] = {0, 0, 0};
    for (int i = 0; i < CIE_SAMPLES_NUMBER; ++i)
    {
        Float val = InterpolateSpectrumSamples(lambda, v, n, CIE_lambda[i]);
        xyz[0] += val * CIE_X[i];
        xyz[1] += val * CIE_Y[i];
        xyz[2] += val * CIE_Z[i];
    }
    Float scale = Float(CIE_lambda[CIE_SAMPLES_NUMBER - 1] - CIE_lambda[0]) /
        Float(CIE_Y_INTEGRAL * CIE_SAMPLES_NUMBER);
    xyz[0] *= scale;
    xyz[1] *= scale;
    xyz[2] *= scale;
    return FromXYZ(xyz);
}


}
}