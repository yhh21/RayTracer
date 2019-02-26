#include "TabulatedBSSRDF.h"

namespace core
{
namespace bxdf
{


core::color::Spectrum TabulatedBSSRDF::Sr(Float r) const
{
    core::color::Spectrum Sr(FLOAT_0);
    for (int ch = 0; ch < core::color::Spectrum::SAMPLE_NUMBER; ++ch)
    {
        // Convert $r$ into unitless optical radius $r_{\roman{optical}}$
        Float rOptical = r * sigma_t[ch];

        // Compute spline weights to interpolate BSSRDF on channel _ch_
        int rhoOffset, radiusOffset;
        Float rhoWeights[4], radiusWeights[4];
        if (!CatmullRomWeights(table.nRhoSamples, table.rhoSamples.get(),
            rho[ch], &rhoOffset, rhoWeights) ||
            !CatmullRomWeights(table.nRadiusSamples, table.radiusSamples.get(),
                rOptical, &radiusOffset, radiusWeights))
        {
            continue;
        }

        // Set BSSRDF value _Sr[ch]_ using tensor spline interpolation
        Float sr = FLOAT_0;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                Float weight = rhoWeights[i] * radiusWeights[j];
                if (weight != FLOAT_0)
                {
                    sr += weight *
                        table.EvalProfile(rhoOffset + i, radiusOffset + j);
                }
            }
        }

        // Cancel marginal PDF factor from tabulated BSSRDF profile
        if (rOptical != FLOAT_0)
        {
            sr /= FLOAT_2 * common::math::PI * rOptical;
        }
        Sr[ch] = sr;
    }
    // Transform BSSRDF value into world space units
    Sr *= sigma_t * sigma_t;
    return Sr.Clamp();
}

Float TabulatedBSSRDF::Sample_Sr(int ch, Float u) const
{
    if (FLOAT_0 == sigma_t[ch])
    {
        return -FLOAT_1;
    }
    return SampleCatmullRom2D(table.nRhoSamples, table.nRadiusSamples,
        table.rhoSamples.get(), table.radiusSamples.get(),
        table.profile.get(), table.profileCDF.get(),
        rho[ch], u) /
        sigma_t[ch];
}

Float TabulatedBSSRDF::Pdf_Sr(int ch, Float r) const
{
    // Convert $r$ into unitless optical radius $r_{\roman{optical}}$
    Float rOptical = r * sigma_t[ch];

    // Compute spline weights to interpolate BSSRDF density on channel _ch_
    int rhoOffset, radiusOffset;
    Float rhoWeights[4], radiusWeights[4];
    if (!CatmullRomWeights(table.nRhoSamples, table.rhoSamples.get(), rho[ch],
        &rhoOffset, rhoWeights) ||
        !CatmullRomWeights(table.nRadiusSamples, table.radiusSamples.get(),
            rOptical, &radiusOffset, radiusWeights))
    {
        return FLOAT_0;
    }

    // Return BSSRDF profile density for channel _ch_
    Float sr = FLOAT_0, rhoEff = FLOAT_0;
    for (int i = 0; i < 4; ++i)
    {
        if (FLOAT_0 == rhoWeights[i])
        {
            continue;
        }
        rhoEff += table.rhoEff[rhoOffset + i] * rhoWeights[i];
        for (int j = 0; j < 4; ++j)
        {
            if (FLOAT_0 == radiusWeights[j])
            {
                continue;
            }
            sr += table.EvalProfile(rhoOffset + i, radiusOffset + j) *
                rhoWeights[i] * radiusWeights[j];
        }
    }

    // Cancel marginal PDF factor from tabulated BSSRDF profile
    if (rOptical != FLOAT_0)
    {
        sr /= FLOAT_2 * common::math::PI * rOptical;
    }
    return (std::max)(FLOAT_0, sr * sigma_t[ch] * sigma_t[ch] / rhoEff);
}


}
}