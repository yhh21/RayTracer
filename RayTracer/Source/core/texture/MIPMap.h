#pragma once

#include "Texture.h"
#include "../interaction/SurfaceInteraction.h"
#include "../color/Spectrum.h"
#include "../../common/math/Vec2.h"
#include "../../common/tool/MemoryArena.h"
#include "../../common/tool/MultiThread.h"

namespace core
{
namespace texture
{


enum class ImageWrap
{
    Repeat,
    Black,
    Clamp
};

struct ResampleWeight
{
    int firstTexel;
    Float weight[4];
};

template <typename T>
class MIPMap
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    MIPMap(const common::math::Vec2i &resolution, const T *data, bool doTri = false,
        Float maxAniso = static_cast<Float>(8.0F), ImageWrap wrapMode = ImageWrap::Repeat);


    int Width() const
    {
        return resolution[0];
    }

    int Height() const
    {
        return resolution[1];
    }

    int Levels() const
    {
        return pyramid.size();
    }

    const T &Texel(int level, int s, int t) const;

    T Lookup(const common::math::Vec2f &st, Float width = FLOAT_0) const;

    T Lookup(const common::math::Vec2f &st, common::math::Vec2f dstdx, common::math::Vec2f dstdy) const;

private:

    std::unique_ptr<ResampleWeight[]> resampleWeights(int oldRes, int newRes)
    {
        CHECK_GE(newRes, oldRes);
        std::unique_ptr<ResampleWeight[]> wt(new ResampleWeight[newRes]);
        Float filterwidth = FLOAT_2;
        for (int i = 0; i < newRes; ++i)
        {
            // Compute image resampling weights for _i_th texel
            Float center = (i + FLOAT_INV_2) * oldRes / newRes;
            wt[i].firstTexel = std::floor((center - filterwidth) + FLOAT_INV_2);
            for (int j = 0; j < 4; ++j)
            {
                Float pos = wt[i].firstTexel + j + FLOAT_INV_2;
                wt[i].weight[j] = Lanczos((pos - center) / filterwidth);
            }

            // Normalize filter weights for texel resampling
            Float invSumWts = FLOAT_1 / (wt[i].weight[0] + wt[i].weight[1] + wt[i].weight[2] + wt[i].weight[3]);
            for (int j = 0; j < 4; ++j)
            {
                wt[i].weight[j] *= invSumWts;
            }
        }
        return wt;
    }

    Float clamp(Float v)
    {
        return v < FLOAT_0 ? FLOAT_0 : v;
    }

    T triangle(int level, const common::math::Vec2f &st) const;

    T EWA(int level, common::math::Vec2f st, common::math::Vec2f dst0, common::math::Vec2f dst1) const;


    const bool doTrilinear;
    const Float maxAnisotropy;
    const ImageWrap wrapMode;
    common::math::Vec2i resolution;
    std::vector<std::unique_ptr<common::tool::BlockedArray<T>>> pyramid;
    static constexpr int WeightLUTSize = 128;
    static Float weightLut[WeightLUTSize];
};

template <typename T>
MIPMap<T>::MIPMap(const common::math::Vec2i &res, const T *img, bool doTrilinear,
    Float maxAnisotropy, ImageWrap wrapMode)
    : doTrilinear(doTrilinear),
    maxAnisotropy(maxAnisotropy),
    wrapMode(wrapMode),
    resolution(res)
{
    //ProfilePhase _(Prof::MIPMapCreation);

    std::unique_ptr<T[]> resampledImage = nullptr;
    if (!common::math::IsPowerOf2(resolution[0]) || !common::math::IsPowerOf2(resolution[1]))
    {
        // Resample image to power-of-two resolution
        common::math::Vec2f resPow2(common::math::RoundUpPow2(resolution[0])
            , common::math::RoundUpPow2(resolution[1]));
        //LOG(INFO) << "Resampling MIPMap from " << resolution << " to " <<
        //    resPow2 << ". Ratio= " << (Float(resPow2.x * resPow2.y) /
        //        Float(resolution.x * resolution.y));
        // Resample image in $s$ direction
        std::unique_ptr<ResampleWeight[]> sWeights = resampleWeights(resolution[0], resPow2[0]);
        resampledImage.reset(new T[resPow2[0] * resPow2[1]]);

        // Apply _sWeights_ to zoom in $s$ direction
        common::tool::ParallelFor([&](int t)
        {
            for (int s = 0; s < resPow2[0]; ++s)
            {
                // Compute texel $(s,t)$ in $s$-zoomed image
                resampledImage[t * resPow2[0] + s] = FLOAT_0;
                for (int j = 0; j < 4; ++j)
                {
                    int origS = sWeights[s].firstTexel + j;
                    if (wrapMode == ImageWrap::Repeat)
                    {
                        origS = common::math::Mod(origS, resolution[0]);
                    }
                    else if (wrapMode == ImageWrap::Clamp)
                    {
                        origS = common::math::Clamp(origS, 0, resolution[0] - 1);
                    }
                    if (origS >= 0 && origS < resolution[0])
                    {
                        resampledImage[t * resPow2[0] + s] +=
                            sWeights[s].weight[j] *
                            img[t * resolution[0] + origS];
                    }
                }
            }
        }, resolution[1], 16);

        // Resample image in $t$ direction
        std::unique_ptr<ResampleWeight[]> tWeights =
            resampleWeights(resolution[1], resPow2[1]);
        std::vector<T *> resampleBufs;
        int nThreads = common::tool::MaxThreadIndex();
        for (int i = 0; i < nThreads; ++i)
        {
            resampleBufs.push_back(new T[resPow2[1]]);
        }
        common::tool::ParallelFor([&](int s)
        {
            T *workData = resampleBufs[common::tool::ThreadIndex];
            for (int t = 0; t < resPow2[1]; ++t)
            {
                workData[t] = FLOAT_0;
                for (int j = 0; j < 4; ++j)
                {
                    int offset = tWeights[t].firstTexel + j;
                    if (wrapMode == ImageWrap::Repeat)
                    {
                        offset = common::math::Mod(offset, resolution[1]);
                    }
                    else if (wrapMode == ImageWrap::Clamp)
                    {
                        offset = common::math::Clamp(offset, 0, resolution[1] - 1);
                    }
                    if (offset >= 0 && offset < resolution[1])
                    {
                        workData[t] += tWeights[t].weight[j] *
                            resampledImage[offset * resPow2[0] + s];
                    }
                }
            }
            for (int t = 0; t < resPow2[1]; ++t)
            {
                resampledImage[t * resPow2[0] + s] = clamp(workData[t]);
            }
        }, resPow2[0], 32);
        for (auto ptr : resampleBufs)
        {
            delete[] ptr;
        }
        resolution = resPow2;
    }
    // Initialize levels of MIPMap from image
    int nLevels = 1 + common::math::Log2Int((std::max)(resolution[0], resolution[1]));
    pyramid.resize(nLevels);

    // Initialize most detailed level of MIPMap
    pyramid[0].reset(
        new common::tool::BlockedArray<T>(resolution[0], resolution[1],
            resampledImage ? resampledImage.get() : img));
    for (int i = 1; i < nLevels; ++i)
    {
        // Initialize $i$th MIPMap level from $i-1$st level
        int sRes = (std::max)(1, pyramid[i - 1]->uSize() / 2);
        int tRes = (std::max)(1, pyramid[i - 1]->vSize() / 2);
        pyramid[i].reset(new common::tool::BlockedArray<T>(sRes, tRes));

        // Filter four texels from finer level of pyramid
        common::tool::ParallelFor([&](int t)
        {
            for (int s = 0; s < sRes; ++s)
            {
                (*pyramid[i])(s, t) =
                    .25f * (Texel(i - 1, 2 * s, 2 * t) +
                        Texel(i - 1, 2 * s + 1, 2 * t) +
                        Texel(i - 1, 2 * s, 2 * t + 1) +
                        Texel(i - 1, 2 * s + 1, 2 * t + 1));
            }
        }, tRes, 16);
    }

    // Initialize EWA filter weights if needed
    if (weightLut[0] == FLOAT_0)
    {
        for (int i = 0; i < WeightLUTSize; ++i)
        {
            Float alpha = FLOAT_2;
            Float r2 = static_cast<Float>(i) / (static_cast<Float>(WeightLUTSize) - FLOAT_1);
            weightLut[i] = std::exp(-alpha * r2) - std::exp(-alpha);
        }
    }
    //mipMapMemory += (4 * resolution[0] * resolution[1] * sizeof(T)) / 3;
}

template <typename T>
const T &MIPMap<T>::Texel(int level, int s, int t) const
{
    CHECK_LT(level, pyramid.size());
    const common::tool::BlockedArray<T> &l = *pyramid[level];
    // Compute texel $(s,t)$ accounting for boundary conditions
    switch (wrapMode)
    {
    case ImageWrap::Repeat:
        s = common::math::Mod(s, l.uSize());
        t = common::math::Mod(t, l.vSize());
        break;
    case ImageWrap::Clamp:
        s = common::math::Clamp(s, 0, l.uSize() - 1);
        t = common::math::Clamp(t, 0, l.vSize() - 1);
        break;
    case ImageWrap::Black:
    {
        static const T black = FLOAT_0;
        if (s < 0 || s >= l.uSize() || t < 0 || t >= l.vSize())
        {
            return black;
        }
        break;
    }
    }
    return l(s, t);
}

template <typename T>
T MIPMap<T>::Lookup(const common::math::Vec2f &st, Float width) const
{
    //++nTrilerpLookups;
    //ProfilePhase p(Prof::TexFiltTrilerp);
    // Compute MIPMap level for trilinear filtering
    Float level = Levels() - FLOAT_1 + common::math::Log2((std::max)(width, static_cast<Float>(1e-8)));

    // Perform trilinear interpolation at appropriate MIPMap level
    if (level < FLOAT_0)
    {
        return triangle(0, st);
    }
    else if (level >= Levels() - FLOAT_1)
    {
        return Texel(Levels() - 1, 0, 0);
    }
    else
    {
        int iLevel = std::floor(level);
        Float delta = level - iLevel;
        return common::math::Lerp(delta, triangle(iLevel, st), triangle(iLevel + 1, st));
    }
}

template <typename T>
T MIPMap<T>::triangle(int level, const common::math::Vec2f &st) const
{
    level = common::math::Clamp(level, 0, Levels() - 1);
    Float s = st[0] * pyramid[level]->uSize() - FLOAT_INV_2;
    Float t = st[1] * pyramid[level]->vSize() - FLOAT_INV_2;
    int s0 = std::floor(s), t0 = std::floor(t);
    Float ds = s - s0, dt = t - t0;
    return (FLOAT_1 - ds) * (FLOAT_1 - dt) * Texel(level, s0, t0)
        + (FLOAT_1 - ds) * dt * Texel(level, s0, t0 + 1)
        + ds * (FLOAT_1 - dt) * Texel(level, s0 + 1, t0)
        + ds * dt * Texel(level, s0 + 1, t0 + 1);
}

template <typename T>
T MIPMap<T>::Lookup(const common::math::Vec2f &st, common::math::Vec2f dst0, common::math::Vec2f dst1) const
{
    if (doTrilinear)
    {
        Float width = (std::max)((std::max)(std::abs(dst0[0]), std::abs(dst0[1])),
            (std::max)(std::abs(dst1[0]), std::abs(dst1[1])));
        return Lookup(st, FLOAT_2 * width);
    }
    //++nEWALookups;
    //ProfilePhase p(Prof::TexFiltEWA);
    // Compute ellipse minor and major axes
    if (LengthSquared(dst0) < LengthSquared(dst1))
    {
        std::swap(dst0, dst1);
    }
    Float majorLength = Length(dst0);
    Float minorLength = Length(dst1);

    // Clamp ellipse eccentricity if too large
    if (minorLength * maxAnisotropy < majorLength && minorLength > FLOAT_0)
    {
        Float scale = majorLength / (minorLength * maxAnisotropy);
        dst1 *= scale;
        minorLength *= scale;
    }
    if (FLOAT_0 == minorLength)
    {
        return triangle(0, st);
    }

    // Choose level of detail for EWA lookup and perform EWA filtering
    Float lod = (std::max)(FLOAT_0, Levels() - FLOAT_1 + common::math::Log2(minorLength));
    int ilod = std::floor(lod);
    return common::math::Lerp(lod - static_cast<Float>(ilod)
        , EWA(ilod, st, dst0, dst1), EWA(ilod + 1, st, dst0, dst1));
}

template <typename T>
T MIPMap<T>::EWA(int level, common::math::Vec2f st, common::math::Vec2f dst0, common::math::Vec2f dst1) const
{
    if (level >= Levels())
    {
        return Texel(Levels() - 1, 0, 0);
    }
    // Convert EWA coordinates to appropriate scale for level
    st[0] = st[0] * pyramid[level]->uSize() - FLOAT_INV_2;
    st[1] = st[1] * pyramid[level]->vSize() - FLOAT_INV_2;
    dst0[0] *= pyramid[level]->uSize();
    dst0[1] *= pyramid[level]->vSize();
    dst1[0] *= pyramid[level]->uSize();
    dst1[1] *= pyramid[level]->vSize();

    // Compute ellipse coefficients to bound EWA filter region
    Float A = dst0[1] * dst0[1] + dst1[1] * dst1[1] + FLOAT_1;
    Float B = -FLOAT_2 * (dst0[0] * dst0[1] + dst1[0] * dst1[1]);
    Float C = dst0[0] * dst0[0] + dst1[0] * dst1[0] + 1;
    Float invF = FLOAT_1 / (A * C - B * B * static_cast<Float>(0.25F));
    A *= invF;
    B *= invF;
    C *= invF;

    // Compute the ellipse's $(s,t)$ bounding box in texture space
    Float det = -B * B + static_cast<Float>(4.0F) * A * C;
    Float invDet = FLOAT_1 / det;
    Float uSqrt = std::sqrt(det * C), vSqrt = std::sqrt(A * det);
    int s0 = std::ceil(st[0] - FLOAT_2 * invDet * uSqrt);
    int s1 = std::floor(st[0] + FLOAT_2 * invDet * uSqrt);
    int t0 = std::ceil(st[1] - FLOAT_2 * invDet * vSqrt);
    int t1 = std::floor(st[1] + FLOAT_2 * invDet * vSqrt);

    // Scan over ellipse bound and compute quadratic equation
    T sum(FLOAT_0);
    Float sumWts = FLOAT_0;
    for (int it = t0; it <= t1; ++it)
    {
        Float tt = it - st[1];
        for (int is = s0; is <= s1; ++is)
        {
            Float ss = is - st[0];
            // Compute squared radius and filter texel if inside ellipse
            Float r2 = A * ss * ss + B * ss * tt + C * tt * tt;
            if (r2 < FLOAT_1)
            {
                int index = (std::min)((int)(r2 * WeightLUTSize), WeightLUTSize - 1);
                Float weight = weightLut[index];
                sum += Texel(level, is, it) * weight;
                sumWts += weight;
            }
        }
    }
    return sum / sumWts;
}

template <typename T>
Float MIPMap<T>::weightLut[WeightLUTSize];


}
}