#pragma once

#include "CoefficientSpectrum.h"

namespace core
{
namespace color
{


static constexpr int RGB_SAMPLES_NUMBER = 3;

class RGBSpectrum : public CoefficientSpectrum<RGB_SAMPLES_NUMBER>
{
public:
    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    RGBSpectrum(Float v = FLOAT_0) : CoefficientSpectrum<RGB_SAMPLES_NUMBER>(v)
    {}

    RGBSpectrum(const CoefficientSpectrum<RGB_SAMPLES_NUMBER> &v)
        : CoefficientSpectrum<RGB_SAMPLES_NUMBER>(v)
    {}


    static RGBSpectrum FromRGB(const Float rgb[3], SpectrumType type = SpectrumType::Reflectance)
    {
        RGBSpectrum s;
        s.c[0] = rgb[0];
        s.c[1] = rgb[1];
        s.c[2] = rgb[2];
        return s;
    }

    void ToRGB(Float *rgb) const
    {
        rgb[0] = c[0];
        rgb[1] = c[1];
        rgb[2] = c[2];
    }

    void ToXYZ(Float xyz[3]) const
    {
        RGBToXYZ(c, xyz);
    }

    static RGBSpectrum FromXYZ(const Float xyz[3], SpectrumType type = SpectrumType::Reflectance)
    {
        RGBSpectrum r;
        XYZToRGB(xyz, r.c);
        return r;
    }

    Float y() const
    {
        const Float YWeight[3] = {static_cast<Float>(0.212671f), static_cast<Float>(0.715160f)
            , static_cast<Float>(0.072169f)};
        return YWeight[0] * c[0] + YWeight[1] * c[1] + YWeight[2] * c[2];
    }

    static RGBSpectrum FromSampled(const Float *lambda, const Float *v, int n)
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
};


inline
RGBSpectrum Lerp(Float t, const RGBSpectrum &s1, const RGBSpectrum &s2)
{
    return (FLOAT_1 - t) * s1 + t * s2;
}


}
}