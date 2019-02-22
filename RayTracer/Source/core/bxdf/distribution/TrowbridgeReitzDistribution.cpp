#include "TrowbridgeReitzDistribution.h"
#include "../BxDF.h"

namespace core
{
namespace bxdf
{
namespace distribution
{


static void TrowbridgeReitzSample11(Float cosTheta, Float U1, Float U2,
    Float *slope_x, Float *slope_y)
{
    // special case (normal incidence)
    if (cosTheta > common::math::ONE_MINUS_MACHINE_EPSILON)
    {
        Float r = std::sqrt(U1 / (FLOAT_1 - U1));
        Float phi = static_cast<Float>(6.28318530718F) * U2;
        *slope_x = r * cos(phi);
        *slope_y = r * sin(phi);
        return;
    }

    Float sinTheta = std::sqrt((std::max)(FLOAT_0, FLOAT_1 - cosTheta * cosTheta));
    Float tanTheta = sinTheta / cosTheta;
    Float a = FLOAT_1 / tanTheta;
    Float G1 = FLOAT_2 / (FLOAT_1 + std::sqrt(FLOAT_1 + FLOAT_1 / (a * a)));

    // sample slope_x
    Float A = FLOAT_2 * U1 / G1 - FLOAT_1;
    Float tmp = FLOAT_1 / (A * A - FLOAT_1);
    tmp = (std::min)(tmp, static_cast<Float>(1e10));
    Float B = tanTheta;
    Float D = std::sqrt((std::max)(Float(B * B * tmp * tmp - (A * A - B * B) * tmp), FLOAT_0));
    Float slope_x_1 = B * tmp - D;
    Float slope_x_2 = B * tmp + D;
    *slope_x = (A < FLOAT_0 || slope_x_2 > FLOAT_1 / tanTheta) ? slope_x_1 : slope_x_2;

    // sample slope_y
    Float S;
    if (U2 > FLOAT_INV_2)
    {
        S = FLOAT_1;
        U2 = FLOAT_2 * (U2 - FLOAT_INV_2);
    }
    else
    {
        S = -FLOAT_1;
        U2 = FLOAT_2 * (FLOAT_INV_2 - U2);
    }
    Float z = (U2 * (U2 * (U2 * static_cast<Float>(0.27385F) - static_cast<Float>(0.73369F))
            + static_cast<Float>(0.46341F))) /
        (U2 * (U2 * (U2 * static_cast<Float>(0.093073F) + static_cast<Float>(0.309420F))
            - FLOAT_1 + static_cast<Float>(0.597999F)));
    *slope_y = S * z * std::sqrt(FLOAT_1 + *slope_x * *slope_x);

    CHECK(!std::isinf(*slope_y));
    CHECK(!std::isnan(*slope_y));
}

static common::math::Vec3f TrowbridgeReitzSample(const common::math::Vec3f &wi, Float alpha_x,
    Float alpha_y, Float U1, Float U2)
{
    // 1. stretch wi
    common::math::Vec3f wiStretched = Normalize(common::math::Vec3f(alpha_x * wi.x, alpha_y * wi.y, wi.z));

    // 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
    Float slope_x, slope_y;
    TrowbridgeReitzSample11(CosTheta(wiStretched), U1, U2, &slope_x, &slope_y);

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


Float TrowbridgeReitzDistribution::D(const common::math::Vec3f &wh) const
{
    Float tan2Theta = Tan2Theta(wh);
    if (std::isinf(tan2Theta))
    {
        return FLOAT_0;
    }
    const Float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
    Float e = (Cos2Phi(wh) / (alphax * alphax) + Sin2Phi(wh) / (alphay * alphay)) * tan2Theta;
    return FLOAT_1 / (common::math::PI * alphax * alphay * cos4Theta * (FLOAT_1 + e) * (FLOAT_1 + e));
}

Float TrowbridgeReitzDistribution::Lambda(const common::math::Vec3f &w) const
{
    Float absTanTheta = std::abs(TanTheta(w));
    if (std::isinf(absTanTheta))
    {
        return FLOAT_0;
    }
    // Compute _alpha_ for direction _w_
    Float alpha = std::sqrt(Cos2Phi(w) * alphax * alphax + Sin2Phi(w) * alphay * alphay);
    Float alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
    return (-FLOAT_1 + std::sqrt(FLOAT_1 + alpha2Tan2Theta)) * FLOAT_INV_2;
}

common::math::Vec3f TrowbridgeReitzDistribution::Sample_wh(const common::math::Vec3f &wo,
    const common::math::Vec2f &u) const
{
    common::math::Vec3f wh;
    if (!sampleVisibleArea)
    {
        Float cosTheta = FLOAT_0, phi = (FLOAT_2 * common::math::PI) * u[FLOAT_1];
        if (alphax == alphay)
        {
            Float tanTheta2 = alphax * alphax * u[0] / (FLOAT_1 - u[0]);
            cosTheta = FLOAT_1 / std::sqrt(FLOAT_1 + tanTheta2);
        }
        else
        {
            phi = std::atan(alphay / alphax * std::tan(FLOAT_2 * common::math::PI * u[FLOAT_1]
                    + FLOAT_INV_2 * common::math::PI));
            if (u[FLOAT_1] > FLOAT_INV_2)
            {
                phi += common::math::PI;
            }
            Float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
            const Float alphax2 = alphax * alphax, alphay2 = alphay * alphay;
            const Float alpha2 = FLOAT_1 / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
            Float tanTheta2 = alpha2 * u[0] / (FLOAT_1 - u[0]);
            cosTheta = FLOAT_1 / std::sqrt(FLOAT_1 + tanTheta2);
        }
        Float sinTheta = std::sqrt((std::max)((Float)FLOAT_0, (Float)FLOAT_1 - cosTheta * cosTheta));
        wh = common::math::SphericalDirection(sinTheta, cosTheta, phi);
        if (!SameHemisphere(wo, wh))
        {
            wh = -wh;
        }
    }
    else
    {
        bool flip = wo.z < FLOAT_0;
        wh = TrowbridgeReitzSample(flip ? -wo : wo, alphax, alphay, u[0], u[FLOAT_1]);
        if (flip)
        {
            wh = -wh;
        }
    }
    return wh;
}


}
}
}