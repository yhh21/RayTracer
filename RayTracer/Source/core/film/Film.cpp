#include "Film.h"

#include "../../common/math/Vec2.h"
#include "../../common/math/Bounds2.h"
#include "../color/ColorExtern.h"

namespace core
{
namespace film
{

///TODO


Film::Film(const common::math::Vec2i &resolution, const common::math::Bounds2f &cropWindow,
    std::unique_ptr<core::sampler::filter::Filter> filt, Float diagonal,
    const std::string &filename, Float scale, Float maxSampleLuminance)
    : fullResolution(resolution),
    diagonal(diagonal * static_cast<Float>(0.001F)),
    filter(std::move(filt)),
    filename(filename),
    scale(scale),
    maxSampleLuminance(maxSampleLuminance)
{
    // Compute film image bounds
    croppedPixelBounds =
        common::math::Bounds2i(common::math::Vec2i(static_cast<int>(std::ceil(fullResolution.x * cropWindow.point_min.x)),
            static_cast<int>(std::ceil(fullResolution.y * cropWindow.point_min.y))),
            common::math::Vec2i(static_cast<int>(std::ceil(fullResolution.x * cropWindow.point_max.x)),
                static_cast<int>(std::ceil(fullResolution.y * cropWindow.point_max.y))));
    /*
    LOG(INFO) << "Created film with full resolution " << resolution <<
        ". Crop window of " << cropWindow << " -> croppedPixelBounds " <<
        croppedPixelBounds;
        */

    // Allocate film image storage
    pixels = std::unique_ptr<Pixel[]>(new Pixel[croppedPixelBounds.Area()]);
    //filmPixelMemory += croppedPixelBounds.Area() * sizeof(Pixel);

    // Precompute filter weight table
    int offset = 0;
    for (int y = 0; y < filterTableWidth; ++y)
    {
        for (int x = 0; x < filterTableWidth; ++x, ++offset)
        {
            common::math::Vec2f p;
            p.x = (x + FLOAT_INV_2) * filter->radius.x / filterTableWidth;
            p.y = (y + FLOAT_INV_2) * filter->radius.y / filterTableWidth;
            filterTable[offset] = filter->Evaluate(p);
        }
    }
}


common::math::Bounds2i Film::GetSampleBounds() const
{
    common::math::Bounds2f floatBounds(Floor(common::math::Vec2f(croppedPixelBounds.point_min)
        + common::math::Vec2f(FLOAT_INV_2) - filter->radius)
        , Ceil(common::math::Vec2f(croppedPixelBounds.point_max)
            - common::math::Vec2f(FLOAT_INV_2) + filter->radius));

    return (common::math::Bounds2i)floatBounds;
}

common::math::Bounds2f Film::GetPhysicalExtent() const
{
    Float aspect = (Float)fullResolution.y / (Float)fullResolution.x;
    Float x = std::sqrt(diagonal * diagonal / (FLOAT_1 + aspect * aspect));
    Float y = aspect * x;
    return common::math::Bounds2f(common::math::Vec2f(-x / FLOAT_2, -y / FLOAT_2)
        , common::math::Vec2f(x / FLOAT_2, y / FLOAT_2));
}


std::unique_ptr<FilmTile> Film::GetFilmTile(const common::math::Bounds2i &sampleBounds)
{
    // Bound image pixels that samples in _sampleBounds_ contribute to
    common::math::Vec2f halfPixel = common::math::Vec2f(FLOAT_INV_2);
    common::math::Bounds2f floatBounds = (common::math::Bounds2f)sampleBounds;
    common::math::Vec2i p0 = (common::math::Vec2i)Ceil(floatBounds.point_min - halfPixel - filter->radius);
    common::math::Vec2i p1 = (common::math::Vec2i)Floor(floatBounds.point_max - halfPixel + filter->radius)
        + common::math::Vec2i(1, 1);
    common::math::Bounds2i tilePixelBounds = Intersect(common::math::Bounds2i(p0, p1), croppedPixelBounds);

    return std::unique_ptr<FilmTile>(new FilmTile(tilePixelBounds, filter->radius
        , filterTable, filterTableWidth, maxSampleLuminance));
}

void Film::MergeFilmTile(std::unique_ptr<FilmTile> tile)
{
    //ProfilePhase p(Prof::MergeFilmTile);
    //VLOG(1) << "Merging film tile " << tile->pixelBounds;
    std::lock_guard<std::mutex> lock(_mutex);

    for (auto point : tile->GetPixelBounds())
    {
        const FilmTilePixel &tilePixel = tile->GetPixel(point);
        Pixel &mergePixel = GetPixel(point);
        Float xyz[3];
        tilePixel.contribSum.ToXYZ(xyz);
        for (int i = 0; i < 3; ++i)
        {
            mergePixel.xyz[i] += xyz[i];
        }
        mergePixel.filterWeightSum += tilePixel.filterWeightSum;
    }
}



void Film::SetImage(const color::Spectrum *img) const
{
    int nPixels = croppedPixelBounds.Area();
    for (int i = 0; i < nPixels; ++i)
    {
        Pixel &p = pixels[i];
        img[i].ToXYZ(p.xyz);
        p.filterWeightSum = 1;
        p.splatXYZ[0] = p.splatXYZ[1] = p.splatXYZ[2] = 0;
    }
}

void Film::AddSplat(const common::math::Vec2f &p, color::Spectrum v)
{
    /*
    ProfilePhase pp(Prof::SplatFilm);

    if (v.HasNaNs())
    {
        LOG(ERROR) << StringPrintf("Ignoring splatted spectrum with NaN values "
            "at (%f, %f)", p.x, p.y);
        return;
    }
    else if (v.y() < 0.)
    {
        LOG(ERROR) << StringPrintf("Ignoring splatted spectrum with negative "
            "luminance %f at (%f, %f)", v.y(), p.x, p.y);
        return;
    }
    else if (std::isinf(v.y()))
    {
        LOG(ERROR) << StringPrintf("Ignoring splatted spectrum with infinite "
            "luminance at (%f, %f)", p.x, p.y);
        return;
    }
    */
    if (!InsideExclusive((common::math::Vec2i)p, croppedPixelBounds))
    {
        return;
    }
    auto y = v.y();
    if (y > maxSampleLuminance)
    { 
        v *= maxSampleLuminance / y;
    }
    Float xyz[3];
    v.ToXYZ(xyz);
    Pixel &pixel = GetPixel((common::math::Vec2i)p);
    for (int i = 0; i < 3; ++i)
    {
        // TODO
        //pixel.splatXYZ[i] += xyz[i];
    }
}

// TODO
void Film::WriteImage(Float splatScale)
{
    // Convert image to RGB and compute final pixel values
    //LOG(INFO) << "Converting image to RGB and computing final weighted pixel values";
    std::unique_ptr<Float[]> rgb(new Float[3 * croppedPixelBounds.Area()]);
    int offset = 0;

    for (auto point : croppedPixelBounds)
    {
        // Convert pixel XYZ color to RGB
        Pixel &pixel = GetPixel(point);
        color::XYZToRGB(pixel.xyz, &rgb[3 * offset]);

        // Normalize pixel with weight sum
        Float filterWeightSum = pixel.filterWeightSum;
        if (FLOAT_0 != filterWeightSum)
        {
            Float invWt = FLOAT_1 / filterWeightSum;
            rgb[3 * offset] = (std::max)(FLOAT_0, rgb[3 * offset] * invWt);
            rgb[3 * offset + 1] = (std::max)(FLOAT_0, rgb[3 * offset + 1] * invWt);
            rgb[3 * offset + 2] = (std::max)(FLOAT_0, rgb[3 * offset + 2] * invWt);
        }

        // Add splat value at pixel
        Float splatRGB[3];
        Float splatXYZ[3] = {pixel.splatXYZ[0], pixel.splatXYZ[1], pixel.splatXYZ[2]};
        color::XYZToRGB(splatXYZ, splatRGB);
        rgb[3 * offset] += splatScale * splatRGB[0];
        rgb[3 * offset + 1] += splatScale * splatRGB[1];
        rgb[3 * offset + 2] += splatScale * splatRGB[2];

        // Scale pixel value by _scale_
        rgb[3 * offset] *= scale;
        rgb[3 * offset + 1] *= scale;
        rgb[3 * offset + 2] *= scale;
        ++offset;
    }

    // Write RGB image
    //LOG(INFO) << "Writing image " << filename << " with bounds " << croppedPixelBounds;
    // TODO
    //pbrt::WriteImage(filename, &rgb[0], croppedPixelBounds, fullResolution);
}


void Film::Clear()
{
    for (auto point : croppedPixelBounds)
    {
        Pixel &pixel = GetPixel(point);
        for (int c = 0; c < 3; ++c)
        {
            pixel.splatXYZ[c] = pixel.xyz[c] = FLOAT_0;
        }
        pixel.filterWeightSum = FLOAT_0;
    }
}


// TODO
/*
Film *CreateFilm(const ParamSet &params, std::unique_ptr<filter::Filter> filter)
{
    std::string filename;
    if (PbrtOptions.imageFile != "")
    {
        filename = PbrtOptions.imageFile;
        std::string paramsFilename = params.FindOneString("filename", "");
        if (paramsFilename != "")
            Warning(
                "Output filename supplied on command line, \"%s\" is overriding "
                "filename provided in scene description file, \"%s\".",
                PbrtOptions.imageFile.c_str(), paramsFilename.c_str());
    }
    else
        filename = params.FindOneString("filename", "pbrt.exr");

    int xres = params.FindOneInt("xresolution", 1280);
    int yres = params.FindOneInt("yresolution", 720);
    if (PbrtOptions.quickRender) xres = std::max(1, xres / 4);
    if (PbrtOptions.quickRender) yres = std::max(1, yres / 4);
    common::math::Bounds2f crop;
    int cwi;
    const Float *cr = params.FindFloat("cropwindow", &cwi);
    if (cr && cwi == 4)
    {
        crop.point_min.x = common::math::Clamp(std::min(cr[0], cr[1]), 0.f, 1.f);
        crop.point_max.x = common::math::Clamp(std::max(cr[0], cr[1]), 0.f, 1.f);
        crop.point_min.y = common::math::Clamp(std::min(cr[2], cr[3]), 0.f, 1.f);
        crop.point_max.y = common::math::Clamp(std::max(cr[2], cr[3]), 0.f, 1.f);
    }
    else if (cr)
        Error("%d values supplied for \"cropwindow\". Expected 4.", cwi);
    else
        crop = common::math::Bounds2f(common::math::Vec2f(common::math::Clamp(PbrtOptions.cropWindow[0][0], 0, 1),
            common::math::Clamp(PbrtOptions.cropWindow[1][0], 0, 1)),
            common::math::Vec2f(common::math::Clamp(PbrtOptions.cropWindow[0][1], 0, 1),
                common::math::Clamp(PbrtOptions.cropWindow[1][1], 0, 1)));

    Float scale = params.FindOneFloat("scale", 1.);
    Float diagonal = params.FindOneFloat("diagonal", 35.);
    Float maxSampleLuminance = params.FindOneFloat("maxsampleluminance",
        Infinity);
    return new Film(common::math::Vec2i(xres, yres), crop, std::move(filter), diagonal,
        filename, scale, maxSampleLuminance);
}
*/

}
}