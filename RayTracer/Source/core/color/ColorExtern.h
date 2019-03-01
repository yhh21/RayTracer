#pragma once

#include "../../pch.h"
#include "../../common/math/Constants.h"
#include <algorithm>

namespace core
{
namespace color
{


////////////////////////////////////////////////////////////////////////////////
// extern
////////////////////////////////////////////////////////////////////////////////

enum class SpectrumType
{
    Reflectance,
    Illuminant
};

#define SAMPLED_LAMBDA_START 380
#define SAMPLED_LAMBDA_END 720
#define SPECTRAL_SAMPLES_NUMBER 60

#define CIE_SAMPLES_NUMBER 471

// CIE X function values
extern const Float CIE_X[CIE_SAMPLES_NUMBER];

// CIE Y function values
extern const Float CIE_Y[CIE_SAMPLES_NUMBER];

// CIE Z function values
extern const Float CIE_Z[CIE_SAMPLES_NUMBER];

extern const Float CIE_lambda[CIE_SAMPLES_NUMBER];

static const Float CIE_Y_INTEGRAL = static_cast<Float>(106.856895);

#define RGB_2_SPECT_SAMPLES_NUMBER 32

extern const Float RGB_2_SPECT_LAMBDA[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_REFL_2_SPECT_WHITE[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_REFL_2_SPECT_CYAN[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_REFL_2_SPECT_MAGENTA[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_REFL_2_SPECT_YELLOW[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_REFL_2_SPECT_RED[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_REFL_2_SPECT_GREEN[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_REFL_2_SPECT_BLUE[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_ILLUM_2_SPECT_WHITE[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_ILLUM_2_SPECT_CYAN[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_ILLUM_2_SPECT_MAGENTA[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_ILLUM_2_SPECT_YELLOW[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_ILLUM_2_SPECT_RED[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_ILLUM_2_SPECT_GREEN[RGB_2_SPECT_SAMPLES_NUMBER];

extern const Float RGB_ILLUM_2_SPECT_BLUE[RGB_2_SPECT_SAMPLES_NUMBER];


inline
void XYZToRGB(const Float xyz[3], Float rgb[3])
{
    rgb[0] = static_cast<Float>(3.240479f * xyz[0]) - static_cast<Float>(1.537150f * xyz[1])
        - static_cast<Float>(0.498535f * xyz[2]);
    rgb[1] = static_cast<Float>(-0.969256f * xyz[0]) + static_cast<Float>(1.875991f) * xyz[1]
        + static_cast<Float>(0.041556f) * xyz[2];
    rgb[2] = static_cast<Float>(0.055648f) * xyz[0] - static_cast<Float>(0.204043f) * xyz[1]
        + static_cast<Float>(1.057311f) * xyz[2];
}

inline
void RGBToXYZ(const Float rgb[3], Float xyz[3])
{
    xyz[0] = static_cast<Float>(0.412453f) * rgb[0] + static_cast<Float>(0.357580f) * rgb[1]
        + static_cast<Float>(0.180423f) * rgb[2];
    xyz[1] = static_cast<Float>(0.212671f) * rgb[0] + static_cast<Float>(0.715160f) * rgb[1]
        + static_cast<Float>(0.072169f) * rgb[2];
    xyz[2] = static_cast<Float>(0.019334f) * rgb[0] + static_cast<Float>(0.119193f) * rgb[1]
        + static_cast<Float>(0.950227f) * rgb[2];
}


bool SpectrumSamplesSorted(const Float *lambda, const Float * /*vals*/, int n);

void SortSpectrumSamples(Float *lambda, Float *vals, int n);

Float AverageSpectrumSamples(const Float *lambda, const Float *vals, int n, Float lambdaStart, Float lambdaEnd);

Float InterpolateSpectrumSamples(const Float *lambda, const Float *vals, int n, Float l);


}
}