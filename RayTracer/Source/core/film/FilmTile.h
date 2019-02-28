#pragma once

#include "../../ForwardDeclaration.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Bounds2.h"
#include "../color/Spectrum.h"

class Filter;

namespace core
{
namespace film
{


struct FilmTilePixel
{
    color::Spectrum contribSum = color::Spectrum(FLOAT_0);
    Float filterWeightSum = FLOAT_0;
};


class FilmTile
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    FilmTile(const common::math::Bounds2i &pixelBounds, const common::math::Vec2f &filterRadius,
        const Float *filterTable, int filterTableSize,
        Float maxSampleLuminance)
        : pixelBounds(pixelBounds),
        filterRadius(filterRadius),
        invFilterRadius(FLOAT_1 / filterRadius.x, FLOAT_1 / filterRadius.y),
        filterTable(filterTable),
        filterTableSize(filterTableSize),
        maxSampleLuminance(maxSampleLuminance)
    {
        pixels = std::vector<FilmTilePixel>((std::max)(0, pixelBounds.Area()));
    }


    void AddSample(const common::math::Vec2f &pFilm, color::Spectrum L,
        Float sampleWeight = FLOAT_1)
    {
        //ProfilePhase _(Prof::AddFilmSample);
        if (L.y() > maxSampleLuminance)
            L *= maxSampleLuminance / L.y();
        // Compute sample's raster bounds
        common::math::Vec2f pFilmDiscrete = pFilm - common::math::Vec2f(FLOAT_INV_2);
        common::math::Vec2i p0 = (common::math::Vec2i)Ceil(pFilmDiscrete - filterRadius);
        common::math::Vec2i p1 = (common::math::Vec2i)Floor(pFilmDiscrete + filterRadius)
            + common::math::Vec2i(1, 1);
        p0 = Max(p0, pixelBounds.point_min);
        p1 = Min(p1, pixelBounds.point_max);

        // Loop over filter support and add sample to pixel arrays

        // Precompute $x$ and $y$ filter table offsets
        int *ifx = ALLOCA(int, p1.x - p0.x);
        for (int x = p0.x; x < p1.x; ++x)
        {
            Float fx = std::abs((x - pFilmDiscrete.x) * invFilterRadius.x *
                filterTableSize);
            ifx[x - p0.x] = (std::min)((int)std::floor(fx), filterTableSize - 1);
        }
        int *ify = ALLOCA(int, p1.y - p0.y);
        for (int y = p0.y; y < p1.y; ++y)
        {
            Float fy = std::abs((y - pFilmDiscrete.y) * invFilterRadius.y *
                filterTableSize);
            ify[y - p0.y] = (std::min)((int)std::floor(fy), filterTableSize - 1);
        }
        for (int y = p0.y; y < p1.y; ++y)
        {
            for (int x = p0.x; x < p1.x; ++x)
            {
                // Evaluate filter value at $(x,y)$ pixel
                int offset = ify[y - p0.y] * filterTableSize + ifx[x - p0.x];
                Float filterWeight = filterTable[offset];

                // Update pixel values with filtered sample contribution
                FilmTilePixel &pixel = GetPixel(common::math::Vec2i(x, y));
                pixel.contribSum += L * sampleWeight * filterWeight;
                pixel.filterWeightSum += filterWeight;
            }
        }
    }

    inline
    FilmTilePixel &GetPixel(const common::math::Vec2i &p)
    {
        CHECK(InsideExclusive(p, pixelBounds));
        int width = pixelBounds.point_max.x - pixelBounds.point_min.x;
        int offset = (p.x - pixelBounds.point_min.x) + (p.y - pixelBounds.point_min.y) * width;
        return pixels[offset];
    }

    inline
    const FilmTilePixel &GetPixel(const common::math::Vec2i &p) const
    {
        CHECK(InsideExclusive(p, pixelBounds));
        int width = pixelBounds.point_max.x - pixelBounds.point_min.x;
        int offset = (p.x - pixelBounds.point_min.x) + (p.y - pixelBounds.point_min.y) * width;
        return pixels[offset];
    }

    const common::math::Bounds2i GetPixelBounds() const
    {
        return pixelBounds;
    }

private:
    // FilmTile Private Data
    const common::math::Bounds2i pixelBounds;
    const common::math::Vec2f filterRadius, invFilterRadius;
    const Float *filterTable;
    const int filterTableSize;
    std::vector<FilmTilePixel> pixels;
    const Float maxSampleLuminance;
    friend class Film;
};


}
}