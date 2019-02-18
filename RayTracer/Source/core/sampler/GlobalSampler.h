#pragma once

#include "../../ForwardDeclaration.h"
#include "Sampler.h"
#include "../../common/tool/RandomNumberGenerator.h"

namespace core
{
namespace sampler
{


class GlobalSampler : public Sampler
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    GlobalSampler(int64_t samples_per_pixel) : Sampler(samples_per_pixel)
    {}


    bool StartNextSample();

    void StartPixel(const common::math::Vec2i &);

    bool SetSampleNumber(int64_t sampleNum);


    Float Get1D();

    common::math::Vec2f Get2D();

    virtual int64_t GetIndexForSample(int64_t sampleNum) const = 0;

    virtual Float SampleDimension(int64_t index, int dimension) const = 0;

private:

    int dimension;

    int64_t intervalSampleIndex;

    static const int arrayStartDim = 5;

    int arrayEndDim;
};


}
}