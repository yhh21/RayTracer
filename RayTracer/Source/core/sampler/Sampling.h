#pragma once

#include "../../ForwardDeclaration.h"
#include "../../common/math/Constants.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Vec3.h"
#include "../../common/tool/RandomNumberGenerator.h"

namespace core
{
namespace sampler
{


void StratifiedSample1D(Float *samples, int nsamples, common::tool::RandomNumberGenerator &rng,
    bool jitter = true);

void StratifiedSample2D(common::math::Vec2f *samples, int nx, int ny, common::tool::RandomNumberGenerator &rng,
    bool jitter = true);

void LatinHypercube(Float *samples, int nSamples, int nDim, common::tool::RandomNumberGenerator &rng);


struct Distribution1D
{
    std::vector<Float> func, cdf;

    Float funcInt;

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    Distribution1D(const Float *f, int n) : func(f, f + n), cdf(n + 1)
    {
        // Compute integral of step function at $x_i$
        cdf[0] = 0;
        for (int i = 1; i < n + 1; ++i)
        {
            cdf[i] = cdf[i - 1] + func[i - 1] / n;
        }

        // Transform step function integral into CDF
        funcInt = cdf[n];
        if (funcInt == 0)
        {
            for (int i = 1; i < n + 1; ++i)
            {
                cdf[i] = static_cast<Float>(i) / static_cast<Float>(n);
            }
        }
        else
        {
            for (int i = 1; i < n + 1; ++i)
            {
                cdf[i] /= funcInt;
            }
        }
    }


    int Count() const
    {
        return (int)func.size();
    }

    Float SampleContinuous(Float u, Float *pdf, int *off = nullptr) const
    {
        // Find surrounding CDF segments and _offset_
        int offset = common::math::FindInterval((int)cdf.size(),
            [&](int index)
        {
            return cdf[index] <= u;
        });
        if (off)
        {
            *off = offset;
        }
        // Compute offset along CDF segment
        Float du = u - cdf[offset];
        if ((cdf[offset + 1] - cdf[offset]) > FLOAT_0)
        {
            CHECK_GT(cdf[offset + 1], cdf[offset]);
            du /= (cdf[offset + 1] - cdf[offset]);
        }
        CHECK(!std::isnan(du));

        // Compute PDF for sampled offset
        if (pdf)
        {
            *pdf = (funcInt > 0) ? func[offset] / funcInt : 0;
        }

        // Return $x\in{}[0,1)$ corresponding to sample
        return (offset + du) / Count();
    }

    int SampleDiscrete(Float u, Float *pdf = nullptr,
        Float *uRemapped = nullptr) const
    {
        // Find surrounding CDF segments and _offset_
        int offset = common::math::FindInterval((int)cdf.size(),
            [&](int index)
        {
            return cdf[index] <= u;
        });
        if (pdf)
        {
            *pdf = (funcInt > 0) ? func[offset] / (funcInt * Count()) : FLOAT_0;
        }
        if (uRemapped)
        {
            *uRemapped = (u - cdf[offset]) / (cdf[offset + 1] - cdf[offset]);
        }
        if (uRemapped)
        {
            CHECK(*uRemapped >= FLOAT_0 && *uRemapped <= FLOAT_1);
        }
        return offset;
    }

    Float DiscretePDF(int index) const
    {
        CHECK(index >= 0 && index < Count());
        return func[index] / (funcInt * Count());
    }

};

common::math::Vec2f RejectionSampleDisk(common::tool::RandomNumberGenerator &rng);

common::math::Vec3f UniformSampleHemisphere(const common::math::Vec2f &u);

Float UniformHemispherePdf();

common::math::Vec3f UniformSampleSphere(const common::math::Vec2f &u);

Float UniformSpherePdf();

common::math::Vec3f UniformSampleCone(const common::math::Vec2f &u, Float thetamax);

common::math::Vec3f UniformSampleCone(const common::math::Vec2f &u, Float thetamax, const common::math::Vec3f &x,
    const common::math::Vec3f &y, const common::math::Vec3f &z);

Float UniformConePdf(Float thetamax);

common::math::Vec2f UniformSampleDisk(const common::math::Vec2f &u);

common::math::Vec2f ConcentricSampleDisk(const common::math::Vec2f &u);

common::math::Vec2f UniformSampleTriangle(const common::math::Vec2f &u);


class Distribution2D
{
public:
    // Distribution2D Public Methods
    Distribution2D(const Float *data, int nu, int nv);

    common::math::Vec2f SampleContinuous(const common::math::Vec2f &u, Float *pdf) const
    {
        Float pdfs[2];
        int v;
        Float d1 = pMarginal->SampleContinuous(u[1], &pdfs[1], &v);
        Float d0 = pConditionalV[v]->SampleContinuous(u[0], &pdfs[0]);
        *pdf = pdfs[0] * pdfs[1];
        return common::math::Vec2f(d0, d1);
    }

    Float Pdf(const common::math::Vec2f &p) const
    {
        int iu = common::math::Clamp(int(p[0] * pConditionalV[0]->Count()), 0,
            pConditionalV[0]->Count() - 1);
        int iv =
            common::math::Clamp(int(p[1] * pMarginal->Count()), 0, pMarginal->Count() - 1);
        return pConditionalV[iv]->func[iu] / pMarginal->funcInt;
    }

private:
    // Distribution2D Private Data
    std::vector<std::unique_ptr<Distribution1D>> pConditionalV;
    std::unique_ptr<Distribution1D> pMarginal;
};


// Sampling Inline Functions
template <typename T>
void Shuffle(T *samp, int count, int nDimensions, common::tool::RandomNumberGenerator &rng)
{
    for (int i = 0; i < count; ++i)
    {
        int other = i + rng.UniformUInt32(count - i);
        for (int j = 0; j < nDimensions; ++j)
        {
            std::swap(samp[nDimensions * i + j], samp[nDimensions * other + j]);
        }
    }
}

inline common::math::Vec3f CosineSampleHemisphere(const common::math::Vec2f &u)
{
    common::math::Vec2f d = ConcentricSampleDisk(u);
    Float z = std::sqrt((std::max)(FLOAT_0, FLOAT_1 - d.x * d.x - d.y * d.y));
    return common::math::Vec3f(d.x, d.y, z);
}

inline Float CosineHemispherePdf(Float cosTheta)
{
    return cosTheta * common::math::INV_PI;
}

inline Float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    return (nf * fPdf) / (nf * fPdf + ng * gPdf);
}

inline Float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    Float f = nf * fPdf, g = ng * gPdf;
    return (f * f) / (f * f + g * g);
}


}
}