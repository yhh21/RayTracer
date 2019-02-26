#pragma once

#include "SeparableBSSRDF.h"
#include "BSSRDFTable.h"

namespace core
{
namespace bxdf
{


class TabulatedBSSRDF : public SeparableBSSRDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    TabulatedBSSRDF(const core::interaction::SurfaceInteraction &po, const core::material::Material *material,
        core::material::TransportMode mode, Float eta, const core::color::Spectrum &sigma_a,
        const core::color::Spectrum &sigma_s, const BSSRDFTable &table)
        : SeparableBSSRDF(po, eta, material, mode), table(table)
    {
        sigma_t = sigma_a + sigma_s;
        for (int c = 0; c < core::color::Spectrum::SAMPLE_NUMBER; ++c)
        {
            rho[c] = sigma_t[c] != 0 ? (sigma_s[c] / sigma_t[c]) : 0;
        }
    }


    core::color::Spectrum Sr(Float distance) const;

    Float Pdf_Sr(int ch, Float distance) const;

    Float Sample_Sr(int ch, Float sample) const;

private:

    const BSSRDFTable &table;

    core::color::Spectrum sigma_t, rho;
};


}
}