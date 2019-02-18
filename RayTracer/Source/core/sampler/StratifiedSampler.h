#pragma once

#include "../../ForwardDeclaration.h"
#include "PixelSampler.h"

namespace core
{
namespace sampler
{


class StratifiedSampler : public PixelSampler
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    StratifiedSampler(int xPixelSamples, int yPixelSamples, bool jitterSamples,
        int nSampledDimensions)
        : PixelSampler(xPixelSamples * yPixelSamples, nSampledDimensions),
        xPixelSamples(xPixelSamples),
        yPixelSamples(yPixelSamples),
        jitterSamples(jitterSamples)
    {}

    void StartPixel(const common::math::Vec2i &);

    std::unique_ptr<Sampler> Clone(int seed);

private:

    const int xPixelSamples, yPixelSamples;

    const bool jitterSamples;
};

// TODO
//StratifiedSampler *CreateStratifiedSampler(const ParamSet &params);


}
}