#include "BxDF.h"
#include "../color/Spectrum.h"
#include "../sampler/Sampling.h"

namespace core
{
namespace bxdf
{


color::Spectrum BxDF::Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi, const common::math::Vec2f &u,
    Float *pdf, BxDFType *sampledType) const
{
    // Cosine-sample the hemisphere, flipping the direction if necessary
    *wi = sampler::CosineSampleHemisphere(u);
    if (wo.z < FLOAT_0)
    {
        wi->z *= -FLOAT_1;
    }
    *pdf = Pdf(wo, *wi);
    return f(wo, *wi);
}

color::Spectrum BxDF::rho(const common::math::Vec3f &w, int nSamples, const common::math::Vec2f *u) const
{
    color::Spectrum r(FLOAT_0);
    for (int i = 0; i < nSamples; ++i)
    {
        // Estimate one term of $\rho_\roman{hd}$
        common::math::Vec3f wi;
        Float pdf = FLOAT_0;
        color::Spectrum f = Sample_f(w, &wi, u[i], &pdf);
        if (pdf > FLOAT_0)
        {
            r += f * AbsCosTheta(wi) / pdf;
        }
    }
    return r / static_cast<Float>(nSamples);
}

color::Spectrum BxDF::rho(int nSamples, const common::math::Vec2f *u1, const common::math::Vec2f *u2) const
{
    color::Spectrum r(FLOAT_0);
    for (int i = 0; i < nSamples; ++i)
    {
        // Estimate one term of $\rho_\roman{hh}$
        common::math::Vec3f wo, wi;
        wo = sampler::UniformSampleHemisphere(u1[i]);
        Float pdfo = sampler::UniformHemispherePdf(), pdfi = FLOAT_0;
        color::Spectrum f = Sample_f(wo, &wi, u2[i], &pdfi);
        if (pdfi > FLOAT_0)
        {
            r += f * AbsCosTheta(wi) * AbsCosTheta(wo) / (pdfo * pdfi);
        }
    }
    return r / (common::math::PI * static_cast<Float>(nSamples));
}

Float BxDF::Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
{
    return SameHemisphere(wo, wi) ? AbsCosTheta(wi) * common::math::INV_PI : FLOAT_0;
}


}
}