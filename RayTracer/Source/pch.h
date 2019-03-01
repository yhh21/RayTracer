#pragma once

#pragma warning(disable : 4091)

//#define DirectX12
#ifdef DirectX12
#include "d3dx12.h"
#endif // DirectX12


// https://en.wikipedia.org/wiki/Netpbm_format
#define PPM_FORMAT
#ifdef PPM_FORMAT
#define _CRT_SECURE_NO_WARNINGS
#endif // PPM_FORMAT

//#define NDEBUG
#ifdef NDEBUG
#undef DEBUG
#else
#define DEBUG
#endif // NDEBUG

#include <cassert>
#include "common/tool/DebugTools.h"

// glog formal
#define CHECK_EQ(val1, val2) assert((val1) == (val2))
#define CHECK_NE(val1, val2) assert((val1) != (val2))
#define CHECK_LE(val1, val2) assert((val1) <= (val2))
#define CHECK_LT(val1, val2) assert((val1) < (val2))
#define CHECK_GE(val1, val2) assert((val1) >= (val2))
#define CHECK_GT(val1, val2) assert((val1) > (val2))
#define CHECK(val1) assert(false != (val1))


//#define FLOAT_AS_DOUBLE

#define ALLOCA(TYPE, COUNT) (TYPE *) alloca((COUNT) * sizeof(TYPE))