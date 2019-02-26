#pragma once

#include "../../ForwardDeclaration.h"

namespace core
{
namespace bxdf
{


struct BSSRDFTable
{
    const int nRhoSamples, nRadiusSamples;
    std::unique_ptr<Float[]> rhoSamples, radiusSamples;
    std::unique_ptr<Float[]> profile;
    std::unique_ptr<Float[]> rhoEff;
    std::unique_ptr<Float[]> profileCDF;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    BSSRDFTable(int nRhoSamples, int nRadiusSamples);


    inline Float EvalProfile(int rhoIndex, int radiusIndex) const
    {
        return profile[rhoIndex * nRadiusSamples + radiusIndex];
    }
};


}
}