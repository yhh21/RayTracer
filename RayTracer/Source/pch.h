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


#define DEBUG
#ifdef DEBUG
#include "DebugTools.hpp"
#endif /// DEBUG

#include <cassert>

using namespace std;