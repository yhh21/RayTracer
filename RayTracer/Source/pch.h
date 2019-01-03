#pragma once

//#define DirectX12
#ifdef DirectX12
#include "d3dx12.h"
#endif /// DirectX12


/// https://en.wikipedia.org/wiki/Netpbm_format
#define PPM_FORMAT
#ifdef PPM_FORMAT
#define _CRT_SECURE_NO_WARNINGS
#endif /// PPM_FORMAT


#define NDEBUG
#ifdef NDEBUG
#else

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

#endif /// NDEBUG


//#define FLOAT_AS_DOUBLE
#ifdef FLOAT_AS_DOUBLE
typedef double Float;
#else
typedef float Float;
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


#define TEST_COMPILING
#ifdef TEST_COMPILING
#include "TestCompiling.h"
#endif // TEST_COMPILING