#include "PixelSampler.h"

namespace core
{
namespace sampler
{


PixelSampler::PixelSampler(int64_t samplesPerPixel, int nSampledDimensions)
    : Sampler(samplesPerPixel)
{
    for (int i = 0; i < nSampledDimensions; ++i)
    {
        samples1D.push_back(std::vector<Float>(samplesPerPixel));
        samples2D.push_back(std::vector<common::math::Vec2f>(samplesPerPixel));
    }
}

bool PixelSampler::StartNextSample()
{
    current1DDimension = current2DDimension = 0;
    return Sampler::StartNextSample();
}

bool PixelSampler::SetSampleNumber(int64_t sampleNum)
{
    current1DDimension = current2DDimension = 0;
    return Sampler::SetSampleNumber(sampleNum);
}

Float PixelSampler::Get1D()
{
    //ProfilePhase _(Prof::GetSample);
    CHECK_LT(current_pixel_sample_index, samples_per_pixel);
    if (current1DDimension < samples1D.size())
    {
        return samples1D[current1DDimension++][current_pixel_sample_index];
    }
    else
    {
        return rng.UniformFloat();
    }
}

common::math::Vec2f PixelSampler::Get2D()
{
    //ProfilePhase _(Prof::GetSample);
    CHECK_LT(current1DDimension, samples_per_pixel);
    if (current2DDimension < samples2D.size())
    {
        return samples2D[current2DDimension++][current1DDimension];
    }
    else
    {
        return common::math::Vec2f(rng.UniformFloat(), rng.UniformFloat());
    }
}


}
}