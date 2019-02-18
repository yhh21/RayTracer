#include "GlobalSampler.h"

namespace core
{
namespace sampler
{


void GlobalSampler::StartPixel(const common::math::Vec2i &p)
{
    //ProfilePhase _(Prof::StartPixel);
    Sampler::StartPixel(p);
    dimension = 0;
    intervalSampleIndex = GetIndexForSample(0);
    // Compute _arrayEndDim_ for dimensions used for array samples
    arrayEndDim = arrayStartDim + sample_array_1D.size() + 2 * sample_array_2D.size();

    // Compute 1D array samples for _GlobalSampler_
    for (size_t i = 0; i < samples_1D_array_sizes.size(); ++i)
    {
        int nSamples = samples_1D_array_sizes[i] * samples_per_pixel;
        for (int j = 0; j < nSamples; ++j)
        {
            int64_t index = GetIndexForSample(j);
            sample_array_1D[i][j] = SampleDimension(index, arrayStartDim + i);
        }
    }

    // Compute 2D array samples for _GlobalSampler_
    int dim = arrayStartDim + samples_1D_array_sizes.size();
    for (size_t i = 0; i < samples_2D_array_sizes.size(); ++i)
    {
        int nSamples = samples_2D_array_sizes[i] * samples_per_pixel;
        for (int j = 0; j < nSamples; ++j)
        {
            int64_t idx = GetIndexForSample(j);
            sample_array_2D[i][j].x = SampleDimension(idx, dim);
            sample_array_2D[i][j].y = SampleDimension(idx, dim + 1);
        }
        dim += 2;
    }
    CHECK_EQ(arrayEndDim, dim);
}

bool GlobalSampler::StartNextSample()
{
    dimension = 0;
    intervalSampleIndex = GetIndexForSample(current_pixel_sample_index + 1);
    return Sampler::StartNextSample();
}

bool GlobalSampler::SetSampleNumber(int64_t sampleNum)
{
    dimension = 0;
    intervalSampleIndex = GetIndexForSample(sampleNum);
    return Sampler::SetSampleNumber(sampleNum);
}

Float GlobalSampler::Get1D()
{
    //ProfilePhase _(Prof::GetSample);
    if (dimension >= arrayStartDim && dimension < arrayEndDim)
    {
        dimension = arrayEndDim;
    }

    return SampleDimension(intervalSampleIndex, dimension++);
}

common::math::Vec2f GlobalSampler::Get2D()
{
    //ProfilePhase _(Prof::GetSample);
    if (dimension + 1 >= arrayStartDim && dimension < arrayEndDim)
    {
        dimension = arrayEndDim;
    }
    common::math::Vec2f p(SampleDimension(intervalSampleIndex, dimension),
        SampleDimension(intervalSampleIndex, dimension + 1));
    dimension += 2;
    return p;
}


}
}