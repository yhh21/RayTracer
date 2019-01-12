#pragma once

#include "../../ForwardDeclaration.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Bounds2.h"

#include "FilmTile.h"
#include "filter/Filter.h"

class ParamSet;

namespace core
{
namespace film
{


class Film
{
public:
    
    const common::math::Vec2i fullResolution;
    const Float diagonal;
    std::unique_ptr<filter::Filter> filter;
    const std::string filename;
    common::math::Bounds2i croppedPixelBounds;

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    Film(const common::math::Vec2i &resolution, const common::math::Bounds2f &cropWindow,
        std::unique_ptr<filter::Filter> filter, Float diagonal,
        const std::string &filename, Float scale,
        Float maxSampleLuminance = (std::numeric_limits<Float>::max)());


    common::math::Bounds2i GetSampleBounds() const;

    common::math::Bounds2f GetPhysicalExtent() const;


    std::unique_ptr<FilmTile> GetFilmTile(const common::math::Bounds2i &sampleBounds);

    void MergeFilmTile(std::unique_ptr<FilmTile> tile);


    void SetImage(const color::Spectrum *img) const;

    void AddSplat(const common::math::Vec2f &p, Spectrum v);

    void WriteImage(Float splatScale = 1);


    void Clear();

private:

    struct Pixel
    {
        Pixel()
        {
            xyz[0] = xyz[1] = xyz[2] = filterWeightSum = 0;
        }
        Float xyz[3];
        Float filterWeightSum;
        /// TODO
        //AtomicFloat splatXYZ[3];
        Float pad;
    };

    Pixel& GetPixel(const common::math::Vec2i &p);

    std::unique_ptr<Pixel[]> pixels;
    static constexpr int filterTableWidth = 16;
    Float filterTable[filterTableWidth * filterTableWidth];
    /// TODO
    //std::mutex mutex;
    const Float scale;
    const Float maxSampleLuminance;
};


Film *CreateFilm(const ParamSet &params, std::unique_ptr<filter::Filter> filter);

}
}