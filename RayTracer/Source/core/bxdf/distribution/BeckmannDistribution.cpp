#include "BeckmannDistribution.h"

namespace core
{
namespace bxdf
{
namespace distribution
{


static void BeckmannSample11(Float cosThetaI, Float U1, Float U2,
    Float *slope_x, Float *slope_y)
{
    /* Special case (normal incidence) */
    if (cosThetaI > common::math::ONE_MINUS_MACHINE_EPSILON)
    {
        Float r = std::sqrt(-std::log(FLOAT_1 - U1));
        Float sinPhi = std::sin(FLOAT_2 * common::math::PI * U2);
        Float cosPhi = std::cos(FLOAT_2 * common::math::PI * U2);
        *slope_x = r * cosPhi;
        *slope_y = r * sinPhi;
        return;
    }

    /* The original inversion routine from the paper contained
       discontinuities, which causes issues for QMC integration
       and techniques like Kelemen-style MLT. The following code
       performs a numerical inversion with better behavior */
    Float sinThetaI = std::sqrt((std::max)(FLOAT_0, FLOAT_1 - cosThetaI * cosThetaI));
    Float tanThetaI = sinThetaI / cosThetaI;
    Float cotThetaI = FLOAT_1 / tanThetaI;

    /* Search interval -- everything is parameterized
       in the Erf() domain */
    Float a = -FLOAT_1, c = common::math::Erf(cotThetaI);
    Float sample_x = (std::max)(U1, static_cast<Float>(1e-6f));

    /* Start with a good initial guess */
    // Float b = (1-sample_x) * a + sample_x * c;

    /* We can do better (inverse of an approximation computed in
     * Mathematica) */
    Float thetaI = std::acos(cosThetaI);
    Float fit = FLOAT_1 + thetaI * (static_cast<Float>(-0.876F)
        + thetaI * (static_cast<Float>(0.4265F) - static_cast<Float>(0.0594F) * thetaI));
    Float b = c - (FLOAT_1 + c) * std::pow(FLOAT_1 - sample_x, fit);

    /* Normalization factor for the CDF */
    static const Float SQRT_PI_INV = FLOAT_1 / std::sqrt(common::math::PI);
    Float normalization = FLOAT_1 / (FLOAT_1 + c + SQRT_PI_INV * tanThetaI * std::exp(-cotThetaI * cotThetaI));

    int it = 0;
    while (++it < 10)
    {
        /* Bisection criterion -- the oddly-looking
           Boolean expression are intentional to check
           for NaNs at little additional cost */
        if (!(b >= a && b <= c))
        {
            b = FLOAT_INV_2 * (a + c);
        }

        /* Evaluate the CDF and its derivative
           (i.e. the density function) */
        Float invErf = common::math::ErfInv(b);
        Float value = normalization *
            (FLOAT_1 + b + SQRT_PI_INV * tanThetaI * std::exp(-invErf * invErf)) - sample_x;
        Float derivative = normalization * (FLOAT_1 - invErf * tanThetaI);

        if (std::abs(value) < static_cast<Float>(1e-5f))
        {
            break;
        }

        /* Update bisection intervals */
        if (value > FLOAT_0)
        {
            c = b;
        }
        else
        {
            a = b;
        }

        b -= value / derivative;
    }

    /* Now convert back into a slope value */
    *slope_x = common::math::ErfInv(b);

    /* Simulate Y component */
    *slope_y = common::math::ErfInv(FLOAT_2 * (std::max)(U2, static_cast<Float>(1e-6f) - FLOAT_1));

    CHECK(!std::isinf(*slope_x));
    CHECK(!std::isnan(*slope_x));
    CHECK(!std::isinf(*slope_y));
    CHECK(!std::isnan(*slope_y));
}

static common::math::Vec3f BeckmannSample(const common::math::Vec3f &wi, Float alpha_x, Float alpha_y,
    Float U1, Float U2)
{
    // 1. stretch wi
    common::math::Vec3f wiStretched = Normalize(common::math::Vec3f(alpha_x * wi.x, alpha_y * wi.y, wi.z));

    // 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
    Float slope_x, slope_y;
    BeckmannSample11(CosTheta(wiStretched), U1, U2, &slope_x, &slope_y);

    // 3. rotate
    Float tmp = CosPhi(wiStretched) * slope_x - SinPhi(wiStretched) * slope_y;
    slope_y = SinPhi(wiStretched) * slope_x + CosPhi(wiStretched) * slope_y;
    slope_x = tmp;

    // 4. unstretch
    slope_x = alpha_x * slope_x;
    slope_y = alpha_y * slope_y;

    // 5. compute normal
    return Normalize(common::math::Vec3f(-slope_x, -slope_y, FLOAT_1));
}


Float BeckmannDistribution::D(const common::math::Vec3f &wh) const
{
    Float tan2Theta = Tan2Theta(wh);
    if (std::isinf(tan2Theta))
    {
        return FLOAT_0;
    }

    Float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
    return std::exp(-tan2Theta * (Cos2Phi(wh) / (alphax * alphax) +
        Sin2Phi(wh) / (alphay * alphay))) /
        (common::math::PI * alphax * alphay * cos4Theta);
}

Float BeckmannDistribution::Lambda(const common::math::Vec3f &w) const
{
    Float absTanTheta = std::abs(TanTheta(w));
    if (std::isinf(absTanTheta))
    {
        return FLOAT_0;
    }
    // Compute _alpha_ for direction _w_
    Float alpha = std::sqrt(Cos2Phi(w) * alphax * alphax + Sin2Phi(w) * alphay * alphay);
    Float a = FLOAT_1 / (alpha * absTanTheta);
    if (a >= static_cast<Float>(1.6F))
    {
        return FLOAT_0;
    }
    return (FLOAT_1 - static_cast<Float>(1.259F) * a
        + static_cast<Float>(0.396F) * a * a)
        / (static_cast<Float>(3.535F) * a + static_cast<Float>(2.181F) * a * a);
}

common::math::Vec3f BeckmannDistribution::Sample_wh(const common::math::Vec3f &wo,
    const common::math::Vec2f &u) const
{
    if (!sampleVisibleArea)
    {
        // Sample full distribution of normals for Beckmann distribution

        // Compute $\tan^2 \theta$ and $\phi$ for Beckmann distribution sample
        Float tan2Theta, phi;
        if (alphax == alphay)
        {
            Float logSample = std::log(FLOAT_1 - u[0]);
            CHECK(!std::isinf(logSample));
            tan2Theta = -alphax * alphax * logSample;
            phi = u[1] * FLOAT_2 * common::math::PI;
        }
        else
        {
            // Compute _tan2Theta_ and _phi_ for anisotropic Beckmann
            // distribution
            Float logSample = std::log(FLOAT_1 - u[0]);
            CHECK(!std::isinf(logSample));
            phi = std::atan(alphay / alphax *
                std::tan(FLOAT_2 * common::math::PI * u[1] + FLOAT_INV_2 * common::math::PI));
            if (u[1] > FLOAT_INV_2)
            {
                phi += common::math::PI;
            }
            Float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
            Float alphax2 = alphax * alphax, alphay2 = alphay * alphay;
            tan2Theta = -logSample / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
        }

        // Map sampled Beckmann angles to normal direction _wh_
        Float cosTheta = FLOAT_1 / std::sqrt(FLOAT_1 + tan2Theta);
        Float sinTheta = std::sqrt((std::max)(FLOAT_0, FLOAT_1 - cosTheta * cosTheta));
        common::math::Vec3f wh = common::math::SphericalDirection(sinTheta, cosTheta, phi);
        if (!SameHemisphere(wo, wh))
        {
            wh = -wh;
        }
        return wh;
    }
    else
    {
        // Sample visible area of normals for Beckmann distribution
        common::math::Vec3f wh;
        bool flip = wo.z < FLOAT_0;
        wh = BeckmannSample(flip ? -wo : wo, alphax, alphay, u[0], u[1]);
        if (flip)
        {
            wh = -wh;
        }
        return wh;
    }
}


}
}
}