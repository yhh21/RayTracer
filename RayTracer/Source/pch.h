#pragma once

//#define DirectX12
#ifdef DirectX12
#include "d3dx12.h"
#endif // DirectX12

#define _CRT_SECURE_NO_WARNINGS
// https://en.wikipedia.org/wiki/Netpbm_format
#define PPM_FORMAT

#define DEBUG
#ifdef DEBUG
#include <iostream>
using namespace std;
#endif // DEBUG

// Constants
constexpr float PI = 3.1415926535897932F;
constexpr float HALF_PI = PI / 2;
constexpr float INV_PI = 1 / PI;