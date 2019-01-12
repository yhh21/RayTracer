#include "Sampler.h"

#include "../camera/Camera.h"

namespace core
{
namespace sampler
{


void Sampler::StartPixel(const common::math::Vec2i &p)
{
    currentPixel = p;
    current_pixel_sample_index = 0;
    // Reset array offsets for next pixel sample
    array_1D_offset = array_2D_offset = 0;
}

core::camera::CameraSample Sampler::GetCameraSample(const common::math::Vec2i &pRaster)
{
    core::camera::CameraSample cs;
    cs.pFilm = (common::math::Vec2f)pRaster + Get2D();
    cs.time = Get1D();
    cs.pLens = Get2D();
    return cs;
}


void Sampler::Request1DArray(int n)
{
    CHECK_EQ(RoundCount(n), n);
    samples_1D_array_sizes.push_back(n);
    sample_array_1D.push_back(std::vector<Float>(n * samples_per_pixel));
}

void Sampler::Request2DArray(int n)
{
    CHECK_EQ(RoundCount(n), n);
    samples_2D_array_sizes.push_back(n);
    sample_array_2D.push_back(std::vector<common::math::Vec2f>(n * samples_per_pixel));
}


const Float *Sampler::Get1DArray(int n)
{
    if (array_1D_offset == sample_array_1D.size()) return nullptr;
    CHECK_EQ(samples_1D_array_sizes[array_1D_offset], n);
    CHECK_LT(current_pixel_sample_index, samples_per_pixel);
    return &sample_array_1D[array_1D_offset++][current_pixel_sample_index * n];
}

const common::math::Vec2f *Sampler::Get2DArray(int n)
{
    if (array_2D_offset == sample_array_2D.size()) return nullptr;
    CHECK_EQ(samples_2D_array_sizes[array_2D_offset], n);
    CHECK_LT(current_pixel_sample_index, samples_per_pixel);
    return &sample_array_2D[array_2D_offset++][current_pixel_sample_index * n];
}


bool Sampler::StartNextSample()
{
    /// Reset array offsets for next pixel sample
    array_1D_offset = array_2D_offset = 0;
    return ++current_pixel_sample_index < samples_per_pixel;
}

bool Sampler::SetSampleNumber(uint32_t sample_num)
{
    /// Reset array offsets for next pixel sample
    array_1D_offset = array_2D_offset = 0;
    current_pixel_sample_index = sample_num;
    return current_pixel_sample_index < samples_per_pixel;
}


}
}