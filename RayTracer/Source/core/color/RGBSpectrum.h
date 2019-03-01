#pragma once

#include "CoefficientSpectrum.h"

namespace core
{
namespace color
{


#define RGB_SAMPLES_NUMBER 3

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


    static RGBSpectrum FromRGB(const Float rgb[3], SpectrumType type = SpectrumType::Reflectance);

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

    static RGBSpectrum FromXYZ(const Float xyz[3], SpectrumType type = SpectrumType::Reflectance);

    Float y() const
    {
        const Float YWeight[3] = {static_cast<Float>(0.212671f), static_cast<Float>(0.715160f)
            , static_cast<Float>(0.072169f)};
        return YWeight[0] * c[0] + YWeight[1] * c[1] + YWeight[2] * c[2];
    }

    static RGBSpectrum FromSampled(const Float *lambda, const Float *v, int n);
};


}
}