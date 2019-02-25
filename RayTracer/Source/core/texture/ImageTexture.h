#pragma once

#include "Texture.h"
#include "TextureMapping2D.h"
#include "MIPMap.h"
#include "../interaction/SurfaceInteraction.h"
#include <map>

namespace core
{
namespace texture
{


struct TexInfo
{
    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    TexInfo(const std::string &filename, bool dt, Float ma, ImageWrap wm, Float sc,
        bool gamma)
        : filename(filename),
        doTrilinear(dt),
        maxAniso(ma),
        wrapMode(wm),
        scale(sc),
        gamma(gamma)
    {}


    std::string filename;
    bool doTrilinear;
    Float maxAniso;
    ImageWrap wrapMode;
    Float scale;
    bool gamma;

    bool operator<(const TexInfo &t2) const
    {
        if (filename != t2.filename)
        {
            return filename < t2.filename;
        }
        if (doTrilinear != t2.doTrilinear)
        {
            return doTrilinear < t2.doTrilinear;
        }
        if (maxAniso != t2.maxAniso)
        {
            return maxAniso < t2.maxAniso;
        }
        if (scale != t2.scale)
        {
            return scale < t2.scale;
        }
        if (gamma != t2.gamma)
        {
            return !gamma;
        }

        return wrapMode < t2.wrapMode;
    }
};

template <typename Tmemory, typename Treturn>
class ImageTexture : public Texture<Treturn>
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    ImageTexture(std::unique_ptr<TextureMapping2D> m,
        const std::string &filename, bool doTri, Float maxAniso,
        ImageWrap wm, Float scale, bool gamma);


    static void ClearCache()
    {
        textures.erase(textures.begin(), textures.end());
    }

    Treturn Evaluate(const interaction::SurfaceInteraction &si) const
    {
        common::math::Vec2f dstdx, dstdy;
        common::math::Vec2f st = mapping->Map(si, &dstdx, &dstdy);
        Tmemory mem = mipmap->Lookup(st, dstdx, dstdy);
        Treturn ret;
        convertOut(mem, &ret);
        return ret;
    }

private:

    static MIPMap<Tmemory> *GetTexture(const std::string &filename,
        bool doTrilinear, Float maxAniso,
        ImageWrap wm, Float scale, bool gamma);

    static void convertIn(const core::color::RGBSpectrum &from, core::color::RGBSpectrum *to
        , Float scale, bool gamma)
    {
        for (int i = 0; i < core::color::RGBSpectrum::SAMPLE_NUMBER; ++i)
        {
            (*to)[i] = scale * (gamma ? common::math::InverseGammaCorrect(from[i]) : from[i]);
        }
    }

    static void convertIn(const core::color::RGBSpectrum &from, Float *to
        , Float scale, bool gamma)
    {
        *to = scale * (gamma ? common::math::InverseGammaCorrect(from.y()) : from.y());
    }

    static void convertOut(const core::color::RGBSpectrum &from, core::color::Spectrum *to)
    {
        Float rgb[3];
        from.ToRGB(rgb);
        *to = core::color::Spectrum::FromRGB(rgb);
    }

    static void convertOut(Float from, Float *to)
    {
        *to = from;
    }

    std::unique_ptr<TextureMapping2D> mapping;
    MIPMap<Tmemory> *mipmap;
    static std::map<TexInfo, std::unique_ptr<MIPMap<Tmemory>>> textures;
};

template <typename Tmemory, typename Treturn>
std::map<TexInfo, std::unique_ptr<MIPMap<Tmemory>>> ImageTexture<Tmemory, Treturn>::textures;

extern template class ImageTexture<Float, Float>;
extern template class ImageTexture<core::color::RGBSpectrum, core::color::Spectrum>;

/* TODO
ImageTexture<Float, Float> *CreateImageFloatTexture(const Transform &tex2world,
    const TextureParams &tp);

ImageTexture<core::color::RGBSpectrum, Spectrum> *CreateImageSpectrumTexture(
    const Transform &tex2world, const TextureParams &tp);
*/


}
}