#pragma once

#include "BxDF.h"
#include "../color/Spectrum.h"
#include "../material/Material.h"
#include "../../common/math/Vec3.h"

namespace core
{
namespace bxdf
{


class SeparableBSSRDFAdapter : public BxDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    SeparableBSSRDFAdapter(const SeparableBSSRDF *bssrdf)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)), bssrdf(bssrdf)
    {}


    core::color::Spectrum f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
    {
        core::color::Spectrum f = bssrdf->Sw(wi);
        // Update BSSRDF transmission term to account for adjoint light
        // transport
        if (core::material::TransportMode::Radiance == bssrdf->mode)
        {
            f *= bssrdf->eta * bssrdf->eta;
        }
        return f;
    }

private:
    const SeparableBSSRDF *bssrdf;
};


}
}