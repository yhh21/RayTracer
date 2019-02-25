#include "FourierBSDF.h"
#include "Interpolation.h"

namespace core
{
namespace bxdf
{


bool FourierBSDFTable::GetWeightsAndOffset(Float cosTheta, int *offset, Float weights[4]) const
{
    return CatmullRomWeights(nMu, mu, cosTheta, offset, weights);
}


core::color::Spectrum FourierBSDF::f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
{
    // Find the zenith angle cosines and azimuth difference angle
    Float muI = CosTheta(-wi), muO = CosTheta(wo);
    Float cosPhi = CosDPhi(-wi, wo);

    // Compute Fourier coefficients $a_k$ for $(\mui, \muo)$

    // Determine offsets and weights for $\mui$ and $\muo$
    int offsetI, offsetO;
    Float weightsI[4], weightsO[4];
    if (!bsdfTable.GetWeightsAndOffset(muI, &offsetI, weightsI) ||
        !bsdfTable.GetWeightsAndOffset(muO, &offsetO, weightsO))
    {
        return core::color::Spectrum(FLOAT_0);
    }

    // Allocate storage to accumulate _ak_ coefficients
    Float *ak = ALLOCA(Float, bsdfTable.mMax * bsdfTable.nChannels);
    memset(ak, 0, bsdfTable.mMax * bsdfTable.nChannels * sizeof(Float));

    // Accumulate weighted sums of nearby $a_k$ coefficients
    int mMax = 0;
    for (int b = 0; b < 4; ++b)
    {
        for (int a = 0; a < 4; ++a)
        {
            // Add contribution of _(a, b)_ to $a_k$ values
            Float weight = weightsI[a] * weightsO[b];
            if (weight != FLOAT_0)
            {
                int m;
                const Float *ap = bsdfTable.GetAk(offsetI + a, offsetO + b, &m);
                mMax = (std::max)(mMax, m);
                for (int c = 0; c < bsdfTable.nChannels; ++c)
                {
                    for (int k = 0; k < m; ++k)
                    {
                        ak[c * bsdfTable.mMax + k] += weight * ap[c * m + k];
                    }
                }
            }
        }
    }

    // Evaluate Fourier expansion for angle $\phi$
    Float Y = (std::max)(FLOAT_0, Fourier(ak, mMax, cosPhi));
    Float scale = muI != FLOAT_0 ? (FLOAT_1 / std::abs(muI)) : FLOAT_0;

    // Update _scale_ to account for adjoint light transport
    if (mode == core::material::TransportMode::Radiance && muI * muO > FLOAT_0)
    {
        float eta = muI > FLOAT_0 ? FLOAT_1 / bsdfTable.eta : bsdfTable.eta;
        scale *= eta * eta;
    }
    if (bsdfTable.nChannels == 1)
    {
        return core::color::Spectrum(Y * scale);
    }
    else
    {
        // Compute and return RGB colors for tabulated BSDF
        Float R = Fourier(ak + 1 * bsdfTable.mMax, mMax, cosPhi);
        Float B = Fourier(ak + 2 * bsdfTable.mMax, mMax, cosPhi);
        Float G = static_cast<Float>(1.39829F) * Y - static_cast<Float>(0.100913F) * B
            - static_cast<Float>(0.297375F) * R;
        Float rgb[3] = {R * scale, G * scale, B * scale};
        return core::color::Spectrum::FromRGB(rgb).Clamp();
    }
}

core::color::Spectrum FourierBSDF::Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi,
    const common::math::Vec2f &u, Float *pdf,
    BxDFType *sampledType) const
{
    // Sample zenith angle component for _FourierBSDF_
    Float muO = CosTheta(wo);
    Float pdfMu;
    Float muI = SampleCatmullRom2D(bsdfTable.nMu, bsdfTable.nMu, bsdfTable.mu,
        bsdfTable.mu, bsdfTable.a0, bsdfTable.cdf,
        muO, u[1], nullptr, &pdfMu);

    // Compute Fourier coefficients $a_k$ for $(\mui, \muo)$

    // Determine offsets and weights for $\mui$ and $\muo$
    int offsetI, offsetO;
    Float weightsI[4], weightsO[4];
    if (!bsdfTable.GetWeightsAndOffset(muI, &offsetI, weightsI) ||
        !bsdfTable.GetWeightsAndOffset(muO, &offsetO, weightsO))
    {
        return core::color::Spectrum(FLOAT_0);
    }

    // Allocate storage to accumulate _ak_ coefficients
    Float *ak = ALLOCA(Float, bsdfTable.mMax * bsdfTable.nChannels);
    memset(ak, 0, bsdfTable.mMax * bsdfTable.nChannels * sizeof(Float));

    // Accumulate weighted sums of nearby $a_k$ coefficients
    int mMax = 0;
    for (int b = 0; b < 4; ++b)
    {
        for (int a = 0; a < 4; ++a)
        {
            // Add contribution of _(a, b)_ to $a_k$ values
            Float weight = weightsI[a] * weightsO[b];
            if (weight != FLOAT_0)
            {
                int m;
                const Float *ap = bsdfTable.GetAk(offsetI + a, offsetO + b, &m);
                mMax = (std::max)(mMax, m);
                for (int c = 0; c < bsdfTable.nChannels; ++c)
                {
                    for (int k = 0; k < m; ++k)
                    {
                        ak[c * bsdfTable.mMax + k] += weight * ap[c * m + k];
                    }
                }
            }
        }
    }

    // Importance sample the luminance Fourier expansion
    Float phi, pdfPhi;
    Float Y = SampleFourier(ak, bsdfTable.recip, mMax, u[0], &pdfPhi, &phi);
    *pdf = (std::max)(FLOAT_0, pdfPhi * pdfMu);

    // Compute the scattered direction for _FourierBSDF_
    Float sin2ThetaI = (std::max)(FLOAT_0, FLOAT_1 - muI * muI);
    Float norm = std::sqrt(sin2ThetaI / Sin2Theta(wo));
    if (std::isinf(norm))
    {
        norm = FLOAT_0;
    }
    Float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
    *wi = -common::math::Vec3f(norm * (cosPhi * wo.x - sinPhi * wo.y),
        norm * (sinPhi * wo.x + cosPhi * wo.y), muI);

    // Mathematically, wi will be normalized (if wo was). However, in
    // practice, floating-point rounding error can cause some error to
    // accumulate in the computed value of wi here. This can be
    // catastrophic: if the ray intersects an object with the FourierBSDF
    // again and the wo (based on such a wi) is nearly perpendicular to the
    // surface, then the wi computed at the next intersection can end up
    // being substantially (like 4x) longer than normalized, which leads to
    // all sorts of errors, including negative spectral values. Therefore,
    // we normalize again here.
    *wi = Normalize(*wi);

    // Evaluate remaining Fourier expansions for angle $\phi$
    Float scale = muI != FLOAT_0 ? (FLOAT_1 / std::abs(muI)) : FLOAT_0;
    if (mode == core::material::TransportMode::Radiance && muI * muO > FLOAT_0)
    {
        float eta = muI > FLOAT_0 ? FLOAT_1 / bsdfTable.eta : bsdfTable.eta;
        scale *= eta * eta;
    }

    if (bsdfTable.nChannels == 1) return core::color::Spectrum(Y * scale);
    Float R = Fourier(ak + 1 * bsdfTable.mMax, mMax, cosPhi);
    Float B = Fourier(ak + 2 * bsdfTable.mMax, mMax, cosPhi);
    Float G = static_cast<Float>(1.39829F) * Y - static_cast<Float>(0.100913F) * B
        - static_cast<Float>(0.297375F) * R;
    Float rgb[3] = {R * scale, G * scale, B * scale};
    return core::color::Spectrum::FromRGB(rgb).Clamp();
}

Float FourierBSDF::Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const
{
    // Find the zenith angle cosines and azimuth difference angle
    Float muI = CosTheta(-wi), muO = CosTheta(wo);
    Float cosPhi = CosDPhi(-wi, wo);

    // Compute luminance Fourier coefficients $a_k$ for $(\mui, \muo)$
    int offsetI, offsetO;
    Float weightsI[4], weightsO[4];
    if (!bsdfTable.GetWeightsAndOffset(muI, &offsetI, weightsI) ||
        !bsdfTable.GetWeightsAndOffset(muO, &offsetO, weightsO))
    {
        return FLOAT_0;
    }
    Float *ak = ALLOCA(Float, bsdfTable.mMax);
    memset(ak, 0, bsdfTable.mMax * sizeof(Float));
    int mMax = 0;
    for (int o = 0; o < 4; ++o)
    {
        for (int i = 0; i < 4; ++i)
        {
            Float weight = weightsI[i] * weightsO[o];
            if (FLOAT_0 == weight)
            {
                continue;
            }

            int order;
            const Float *coeffs = bsdfTable.GetAk(offsetI + i, offsetO + o, &order);
            mMax = (std::max)(mMax, order);

            for (int k = 0; k < order; ++k)
            {
                ak[k] += *coeffs++ * weight;
            }
        }
    }

    // Evaluate probability of sampling _wi_
    Float rho = FLOAT_0;
    for (int o = 0; o < 4; ++o)
    {
        if (FLOAT_0 == weightsO[o])
        {
            continue;
        }
        rho += weightsO[o] *
            bsdfTable.cdf[(offsetO + o) * bsdfTable.nMu + bsdfTable.nMu - 1] *
            (FLOAT_2 * common::math::PI);
    }
    Float Y = Fourier(ak, mMax, cosPhi);
    return (rho > FLOAT_0 && Y > FLOAT_0) ? (Y / rho) : FLOAT_0;
}


Float Fourier(const Float *a, int m, double cosPhi)
{
    double value = 0.0f;
    // Initialize cosine iterates
    double cosKMinusOnePhi = cosPhi;
    double cosKPhi = 1.0f;
    for (int k = 0; k < m; ++k)
    {
        // Add the current summand and update the cosine iterates
        value += a[k] * cosKPhi;
        double cosKPlusOnePhi = 2.0f * cosPhi * cosKPhi - cosKMinusOnePhi;
        cosKMinusOnePhi = cosKPhi;
        cosKPhi = cosKPlusOnePhi;
    }

    return static_cast<Float>(value);
}

Float SampleFourier(const Float *ak, const Float *recip, int m, Float u, Float *pdf, Float *phiPtr)
{
    // Pick a side and declare bisection variables
    bool flip = (u >= FLOAT_INV_2);
    if (flip)
    {
        u = FLOAT_1 - FLOAT_2 * (u - FLOAT_INV_2);
    }
    else
    {
        u *= FLOAT_2;
    }
    double a = 0, b = common::math::PI, phi = 0.5f * common::math::PI;
    double F, f;
    while (true)
    {
        // Evaluate $F(\phi)$ and its derivative $f(\phi)$

        // Initialize sine and cosine iterates
        double cosPhi = std::cos(phi);
        double sinPhi2 = 1.0f - cosPhi * cosPhi;
        double sinPhi = std::sqrt(sinPhi2 > 0.0f ? sinPhi2 : 0.0f);
        double cosPhiPrev = cosPhi, cosPhiCur = 0.0f;
        double sinPhiPrev = -sinPhi, sinPhiCur = 1.0f;

        // Initialize _F_ and _f_ with the first series term
        F = ak[0] * phi;
        f = ak[0];
        for (int k = 1; k < m; ++k)
        {
            // Compute next sine and cosine iterates
            double sinPhiNext = 2.0f * cosPhi * sinPhiCur - sinPhiPrev;
            double cosPhiNext = 2.0f * cosPhi * cosPhiCur - cosPhiPrev;
            sinPhiPrev = sinPhiCur;
            sinPhiCur = sinPhiNext;
            cosPhiPrev = cosPhiCur;
            cosPhiCur = cosPhiNext;

            // Add the next series term to _F_ and _f_
            F += ak[k] * recip[k] * sinPhiNext;
            f += ak[k] * cosPhiNext;
        }
        F -= u * ak[0] * common::math::PI;

        // Update bisection bounds using updated $\phi$
        if (F > 0.0f)
        {
            b = phi;
        }
        else
        {
            a = phi;
        }

        // Stop the Fourier bisection iteration if converged
        if (std::abs(F) < 1e-6f || b - a < 1e-6f)
        {
            break;
        }

        // Perform a Newton step given $f(\phi)$ and $F(\phi)$
        phi -= F / f;

        // Fall back to a bisection step when $\phi$ is out of bounds
        if (!(phi > a && phi < b))
        {
            phi = FLOAT_INV_2 * (a + b);
        }
    }
    // Potentially flip $\phi$ and return the result
    if (flip)
    {
        phi = 2.0f * common::math::PI - phi;
    }
    *pdf = static_cast<Float>(common::math::INV_TWO_PI * f / ak[0]);
    *phiPtr = static_cast<Float>(phi);
    return static_cast<Float>(f);
}


}
}