#pragma once

#include "../../ForwardDeclaration.h"
#include "../../common/math/Vec2.h"

namespace core
{
namespace sampler
{


class Sampler
{
public:

    const uint32_t samples_per_pixel;

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    Sampler(uint32_t samples_per_pixel) : samples_per_pixel(samples_per_pixel)
    {}

    ~Sampler()
    {}


    virtual std::unique_ptr<Sampler> Clone(int seed) = 0;


    virtual Float Get1D() = 0;

    virtual common::math::Vec2f Get2D() = 0;


    virtual void StartPixel(const common::math::Vec2i &p);

    core::camera::CameraSample GetCameraSample(const common::math::Vec2i &pRaster);

    void Request1DArray(int n);

    void Request2DArray(int n);

    virtual int RoundCount(int n) const
    {
        return n;
    }

    const Float *Get1DArray(int n);

    const common::math::Vec2f *Get2DArray(int n);


    virtual bool StartNextSample();

    virtual bool SetSampleNumber(uint32_t sample_num);

    uint32_t CurrentSampleNumber() const
    {
        return current_pixel_sample_index;
    }

protected:

    common::math::Vec2i currentPixel;
    uint32_t current_pixel_sample_index;
    std::vector<int> samples_1D_array_sizes, samples_2D_array_sizes;
    std::vector<std::vector<Float>> sample_array_1D;
    std::vector<std::vector<common::math::Vec2f>> sample_array_2D;

private:

    size_t array_1D_offset, array_2D_offset;
};


}
}