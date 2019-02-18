#pragma once

#include "../../ForwardDeclaration.h"
#include "Sampler.h"
#include "../../common/tool/RandomNumberGenerator.h"

namespace core
{
namespace sampler
{


class PixelSampler : public Sampler
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    PixelSampler(int64_t samples_per_pixel, int nSampledDimensions);


    bool StartNextSample();

    bool SetSampleNumber(int64_t);

    Float Get1D();

    common::math::Vec2f Get2D();

protected:

    std::vector<std::vector<Float>> samples1D;

    std::vector<std::vector<common::math::Vec2f>> samples2D;

    int current1DDimension = 0, current2DDimension = 0;

    common::tool::RandomNumberGenerator rng;
};


}
}