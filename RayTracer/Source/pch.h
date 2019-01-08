#pragma once

#pragma warning(disable : 4091)

//#define DirectX12
#ifdef DirectX12
#include "d3dx12.h"
#endif /// DirectX12


/// https://en.wikipedia.org/wiki/Netpbm_format
#define PPM_FORMAT
#ifdef PPM_FORMAT
#define _CRT_SECURE_NO_WARNINGS
#endif /// PPM_FORMAT

//#define NDEBUG
#ifdef NDEBUG
#undef DEBUG
#else

#undef NDEBUG
#define DEBUG

#include <cassert>
#include "common/tool/DebugTools.h"

/// glog formal
#define CHECK_EQ(val1, val2) assert(val1 == val2)
#define CHECK_NE(val1, val2) assert(val1 != val2)
#define CHECK_LE(val1, val2) assert(val1 <= val2)
#define CHECK_LT(val1, val2) assert(val1 < val2)
#define CHECK_GE(val1, val2) assert(val1 >= val2)
#define CHECK_GT(val1, val2) assert(val1 > val2)
#define CHECK(val1) assert(true == val1)
#endif /// NDEBUG


//#define FLOAT_AS_DOUBLE
#ifdef FLOAT_AS_DOUBLE
typedef double Float;
#define FLOAT_0 0.0f
#define FLOAT_1 1.0f
#else
typedef float Float;
#define FLOAT_0 0.0F
#define FLOAT_1 1.0F
#endif


template <int SPECTRUM_SAMPLE_NUMBER>
class CoefficientSpectrum;
class RGBSpectrum;
class SampledSpectrum;

#define SAMPLED_SPECTRUM
#ifdef SAMPLED_SPECTRUM
typedef SampledSpectrum Spectrum;
#else
typedef RGBSpectrum Spectrum;
#endif


using namespace std;