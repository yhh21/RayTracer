#include "Texture.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Constants.h"
#include "../interaction/SurfaceInteraction.h"

namespace core
{
namespace texture
{


inline Float SmoothStep(Float min, Float max, Float value)
{
    Float v = common::math::Clamp((value - min) / (max - min), FLOAT_0, FLOAT_1);
    return v * v * (-FLOAT_2 * v + FLOAT_3);
}

inline
Float Grad(int x, int y, int z, Float dx, Float dy, Float dz);

inline
Float NoiseWeight(Float t);


static constexpr int NoisePermSize = 256;

static int NoisePerm[2 * NoisePermSize] = {
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140,
    36, 103, 30, 69, 142,
    // Remainder of the noise permutation table
    8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62,
    94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174,
    20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77,
    146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55,
    46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76,
    132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100,
    109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147,
    118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28,
    42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101,
    155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232,
    178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12,
    191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31,
    181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66,
    215, 61, 156, 180, 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194,
    233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6,
    148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32,
    57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74,
    165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60,
    211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25,
    63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135,
    130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226,
    250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59,
    227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2,
    44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19,
    98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251,
    34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249,
    14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115,
    121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72,
    243, 141, 128, 195, 78, 66, 215, 61, 156, 180};


Float Noise(Float x, Float y, Float z)
{
    // Compute noise cell coordinates and offsets
    int ix = std::floor(x), iy = std::floor(y), iz = std::floor(z);
    Float dx = x - ix, dy = y - iy, dz = z - iz;

    // Compute gradient weights
    ix &= NoisePermSize - 1;
    iy &= NoisePermSize - 1;
    iz &= NoisePermSize - 1;
    Float w000 = Grad(ix, iy, iz, dx, dy, dz);
    Float w100 = Grad(ix + 1, iy, iz, dx - FLOAT_1, dy, dz);
    Float w010 = Grad(ix, iy + 1, iz, dx, dy - FLOAT_1, dz);
    Float w110 = Grad(ix + 1, iy + 1, iz, dx - FLOAT_1, dy - FLOAT_1, dz);
    Float w001 = Grad(ix, iy, iz + 1, dx, dy, dz - FLOAT_1);
    Float w101 = Grad(ix + 1, iy, iz + 1, dx - FLOAT_1, dy, dz - FLOAT_1);
    Float w011 = Grad(ix, iy + 1, iz + 1, dx, dy - FLOAT_1, dz - FLOAT_1);
    Float w111 = Grad(ix + 1, iy + 1, iz + 1, dx - FLOAT_1, dy - FLOAT_1, dz - FLOAT_1);

    // Compute trilinear interpolation of weights
    Float wx = NoiseWeight(dx), wy = NoiseWeight(dy), wz = NoiseWeight(dz);
    Float x00 = common::math::Lerp(wx, w000, w100);
    Float x10 = common::math::Lerp(wx, w010, w110);
    Float x01 = common::math::Lerp(wx, w001, w101);
    Float x11 = common::math::Lerp(wx, w011, w111);
    Float y0 = common::math::Lerp(wy, x00, x10);
    Float y1 = common::math::Lerp(wy, x01, x11);
    return common::math::Lerp(wz, y0, y1);
}

inline
Float Noise(const common::math::Vec3f &p)
{
    return Noise(p.x, p.y, p.z);
}

inline
Float Grad(int x, int y, int z, Float dx, Float dy, Float dz)
{
    int h = NoisePerm[NoisePerm[NoisePerm[x] + y] + z];
    h &= 15;
    Float u = h < 8 || 12 == h || 13 == h ? dx : dy;
    Float v = h < 4 || 12 == h || 13 == h ? dy : dz;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

inline
Float NoiseWeight(Float t)
{
    Float t3 = t * t * t;
    Float t4 = t3 * t;

    return static_cast<Float>(6.0F) * t4 * t - static_cast<Float>(15.0F) * t4 + static_cast<Float>(10.0F) * t3;
}

Float FBm(const common::math::Vec3f &p, const common::math::Vec3f &dpdx, const common::math::Vec3f &dpdy,
    Float omega, int maxOctaves)
{
    // Compute number of octaves for antialiased FBm
    Float len2 = (std::max)(LengthSquared(dpdx), LengthSquared(dpdy));
    Float n = common::math::Clamp(-FLOAT_1 - FLOAT_INV_2 * common::math::Log2(len2), FLOAT_0, maxOctaves);
    int nInt = std::floor(n);

    // Compute sum of octaves of noise for FBm
    Float sum = FLOAT_0, lambda = FLOAT_1, o = FLOAT_1;
    for (int i = 0; i < nInt; ++i)
    {
        sum += o * Noise(lambda * p);
        lambda *= static_cast<Float>(1.99F);
        o *= omega;
    }
    Float nPartial = n - nInt;
    sum += o * SmoothStep(static_cast<Float>(0.3F), static_cast<Float>(0.7F), nPartial) * Noise(lambda * p);
    return sum;
}

Float Turbulence(const common::math::Vec3f &p, const common::math::Vec3f &dpdx, const common::math::Vec3f &dpdy,
    Float omega, int maxOctaves)
{
    // Compute number of octaves for antialiased FBm
    Float len2 = (std::max)(LengthSquared(dpdx), LengthSquared(dpdy));
    Float n = common::math::Clamp(-FLOAT_1 - FLOAT_INV_2 * common::math::Log2(len2), FLOAT_0
        , static_cast<Float>(maxOctaves));
    int nInt = std::floor(n);

    // Compute sum of octaves of noise for turbulence
    Float sum = FLOAT_0, lambda = FLOAT_1, o = FLOAT_1;
    for (int i = 0; i < nInt; ++i)
    {
        sum += o * std::abs(Noise(lambda * p));
        lambda *= static_cast<Float>(1.99F);
        o *= omega;
    }

    // Account for contributions of clamped octaves in turbulence
    Float nPartial = n - nInt;
    sum += o * common::math::Lerp(SmoothStep(static_cast<Float>(0.3F), static_cast<Float>(0.7F), nPartial)
        , static_cast<Float>(0.2F), std::abs(Noise(lambda * p)));
    for (int i = nInt; i < maxOctaves; ++i)
    {
        sum += o * static_cast<Float>(0.2F);
        o *= omega;
    }
    return sum;
}

// Texture Function Definitions
Float Lanczos(Float x, Float tau)
{
    x = std::abs(x);
    if (x < static_cast<Float>(1e-5f))
    {
        return FLOAT_1;
    }
    if (x > FLOAT_1)
    {
        return FLOAT_0;
    }
    x *= common::math::PI;
    Float s = std::sin(x * tau) / (x * tau);
    Float lanczos = std::sin(x) / x;
    return s * lanczos;
}



}
}