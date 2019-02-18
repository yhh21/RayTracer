#include "Sampling.h"


namespace core
{
namespace sampler
{


void StratifiedSample1D(Float *samp, int nSamples, common::tool::RandomNumberGenerator &rng, bool jitter)
{
    Float invNSamples = FLOAT_1 / nSamples;
    for (int i = 0; i < nSamples; ++i)
    {
        Float delta = jitter ? rng.UniformFloat() : FLOAT_INV_2;
        samp[i] = (std::min)((i + delta) * invNSamples, common::math::ONE_MINUS_MACHINE_EPSILON);
    }
}

void StratifiedSample2D(common::math::Vec2f *samp, int nx, int ny, common::tool::RandomNumberGenerator &rng, bool jitter)
{
    Float dx = FLOAT_1 / nx, dy = FLOAT_1 / ny;
    for (int y = 0; y < ny; ++y)
    {
        for (int x = 0; x < nx; ++x)
        {
            Float jx = jitter ? rng.UniformFloat() : FLOAT_INV_2;
            Float jy = jitter ? rng.UniformFloat() : FLOAT_INV_2;
            samp->x = (std::min)((x + jx) * dx, common::math::ONE_MINUS_MACHINE_EPSILON);
            samp->y = (std::min)((y + jy) * dy, common::math::ONE_MINUS_MACHINE_EPSILON);
            ++samp;
        }
    }
}

void LatinHypercube(Float *samples, int nSamples, int nDim, common::tool::RandomNumberGenerator &rng)
{
    // Generate LHS samples along diagonal
    Float invNSamples = FLOAT_1 / nSamples;
    for (int i = 0; i < nSamples; ++i)
    {
        for (int j = 0; j < nDim; ++j)
        {
            Float sj = (i + (rng.UniformFloat())) * invNSamples;
            samples[nDim * i + j] = (std::min)(sj, common::math::ONE_MINUS_MACHINE_EPSILON);
        }
    }

    // Permute LHS samples in each dimension
    for (int i = 0; i < nDim; ++i)
    {
        for (int j = 0; j < nSamples; ++j)
        {
            int other = j + rng.UniformUInt32(nSamples - j);
            std::swap(samples[nDim * j + i], samples[nDim * other + i]);
        }
    }
}

common::math::Vec2f RejectionSampleDisk(common::tool::RandomNumberGenerator &rng)
{
    common::math::Vec2f p;
    do
    {
        p.x = FLOAT_1 - FLOAT_2 * rng.UniformFloat();
        p.y = FLOAT_1 - FLOAT_2 * rng.UniformFloat();
    }
    while (p.x * p.x + p.y * p.y > FLOAT_1);
    return p;
}

common::math::Vec3f UniformSampleHemisphere(const common::math::Vec2f &u)
{
    Float z = u[0];
    Float r = std::sqrt((std::max)(FLOAT_0, FLOAT_1 - z * z));
    Float phi = FLOAT_2 * common::math::PI * u[1];
    return common::math::Vec3f(r * std::cos(phi), r * std::sin(phi), z);
}

Float UniformHemispherePdf()
{
    return common::math::INV_TWO_PI;
}

common::math::Vec3f UniformSampleSphere(const common::math::Vec2f &u)
{
    Float z = FLOAT_1 - FLOAT_2 * u[0];
    Float r = std::sqrt((std::max)(FLOAT_0, FLOAT_1 - z * z));
    Float phi = FLOAT_2 * common::math::PI * u[1];
    return common::math::Vec3f(r * std::cos(phi), r * std::sin(phi), z);
}

Float UniformSpherePdf()
{
    return common::math::INV_FOUR_PI;
}

common::math::Vec2f UniformSampleDisk(const common::math::Vec2f &u)
{
    Float r = std::sqrt(u[0]);
    Float theta = FLOAT_2 * common::math::PI * u[1];
    return common::math::Vec2f(r * std::cos(theta), r * std::sin(theta));
}

common::math::Vec2f ConcentricSampleDisk(const common::math::Vec2f &u)
{
    // Map uniform random numbers to $[-1,1]^2$
    common::math::Vec2f uOffset = FLOAT_2 * u - common::math::Vec2f(FLOAT_1, FLOAT_1);

    // Handle degeneracy at the origin
    if (uOffset.x == FLOAT_0 && uOffset.y == FLOAT_0)
    {
        return common::math::Vec2f(FLOAT_0, FLOAT_0);
    }

    // Apply concentric mapping to point
    Float theta, r;
    if (std::abs(uOffset.x) > std::abs(uOffset.y))
    {
        r = uOffset.x;
        theta = common::math::PI_DIV_FOUR * (uOffset.y / uOffset.x);
    }
    else
    {
        r = uOffset.y;
        theta = common::math::PI_DIV_TWO - common::math::PI_DIV_FOUR * (uOffset.x / uOffset.y);
    }
    return r * common::math::Vec2f(std::cos(theta), std::sin(theta));
}

Float UniformConePdf(Float cosThetaMax)
{
    return FLOAT_1 / (FLOAT_2 * common::math::PI * (FLOAT_1 - cosThetaMax));
}

common::math::Vec3f UniformSampleCone(const common::math::Vec2f &u, Float cosThetaMax)
{
    Float cosTheta = (FLOAT_1 - u[0]) + u[0] * cosThetaMax;
    Float sinTheta = std::sqrt(FLOAT_1 - cosTheta * cosTheta);
    Float phi = u[1] * FLOAT_2 * common::math::PI;
    return common::math::Vec3f(std::cos(phi) * sinTheta, std::sin(phi) * sinTheta,
        cosTheta);
}

common::math::Vec3f UniformSampleCone(const common::math::Vec2f &u, Float cosThetaMax,
    const common::math::Vec3f &x, const common::math::Vec3f &y,
    const common::math::Vec3f &z)
{
    Float cosTheta = common::math::Lerp(u[0], cosThetaMax, FLOAT_1);
    Float sinTheta = std::sqrt(FLOAT_1 - cosTheta * cosTheta);
    Float phi = u[1] * FLOAT_2 * common::math::PI;
    return std::cos(phi) * sinTheta * x + std::sin(phi) * sinTheta * y +
        cosTheta * z;
}

common::math::Vec2f UniformSampleTriangle(const common::math::Vec2f &u)
{
    Float su0 = std::sqrt(u[0]);
    return common::math::Vec2f(FLOAT_1 - su0, u[1] * su0);
}

Distribution2D::Distribution2D(const Float *func, int nu, int nv)
{
    pConditionalV.reserve(nv);
    for (int v = 0; v < nv; ++v)
    {
        // Compute conditional sampling distribution for $\tilde{v}$
        pConditionalV.emplace_back(new Distribution1D(&func[v * nu], nu));
    }
    // Compute marginal sampling distribution $p[\tilde{v}]$
    std::vector<Float> marginalFunc;
    marginalFunc.reserve(nv);
    for (int v = 0; v < nv; ++v)
    {
        marginalFunc.push_back(pConditionalV[v]->funcInt);
    }
    pMarginal.reset(new Distribution1D(&marginalFunc[0], nv));
}


}
}