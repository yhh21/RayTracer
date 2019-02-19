#include "ColorExtern.h"


namespace core
{
namespace color
{


bool SpectrumSamplesSorted(const Float *lambda, const Float * /*vals*/, int n)
{
    for (int i = 0; i < n - 1; ++i)
    {
        if (lambda[i] > lambda[i + 1]) return false;
    }

    return true;
}

// TODO
void SortSpectrumSamples(Float *lambda, Float *vals, int n)
{
    std::vector<std::pair<Float, Float>> sort_vec;
    sort_vec.reserve(n);
    for (int i = 0; i < n; ++i)
    {
        sort_vec.push_back(std::make_pair(lambda[i], vals[i]));
    }
    std::sort(sort_vec.begin(), sort_vec.end());
    for (int i = 0; i < n; ++i)
    {
        lambda[i] = sort_vec[i].first;
        vals[i] = sort_vec[i].second;
    }
}

// TODO
Float AverageSpectrumSamples(const Float *lambda, const Float *vals, int n,
    Float lambdaStart, Float lambdaEnd)
{
#ifdef DEBUG
    for (int i = 0; i < n - 1; ++i)
    {
        CHECK_GT(lambda[i + 1], lambda[i]);
    }
    CHECK_LT(lambdaStart, lambdaEnd);
#endif // DEBUG
    // Handle cases with out-of-bounds range or single sample only
    if (lambdaEnd <= lambda[0]) return vals[0];
    if (lambdaStart >= lambda[n - 1]) return vals[n - 1];
    if (n == 1) return vals[0];
    Float sum = FLOAT_0;
    // Add contributions of constant segments before/after samples
    if (lambdaStart < lambda[0]) sum += vals[0] * (lambda[0] - lambdaStart);
    if (lambdaEnd > lambda[n - 1])
        sum += vals[n - 1] * (lambdaEnd - lambda[n - 1]);

    // Advance to first relevant wavelength segment
    int i = 0;
    for (; lambdaStart > lambda[i + 1]; ++i);
#ifdef DEBUG
    CHECK_LT(i + 1, n);
#endif // DEBUG

    // Loop over wavelength sample segments and add contributions
    auto interp = [lambda, vals](Float w, int i)
    {
        return common::math::Lerp((w - lambda[i]) / (lambda[i + 1] - lambda[i]), vals[i],
            vals[i + 1]);
    };
    for (; i + 1 < n && lambdaEnd >= lambda[i]; ++i)
    {
        Float segLambdaStart = (std::max)(lambdaStart, lambda[i]);
        Float segLambdaEnd = (std::min)(lambdaEnd, lambda[i + 1]);
        sum += FLOAT_INV_2 * (interp(segLambdaStart, i) + interp(segLambdaEnd, i)) *
            (segLambdaEnd - segLambdaStart);
    }
    return sum / (lambdaEnd - lambdaStart);
}

Float InterpolateSpectrumSamples(const Float *lambda, const Float *vals, int n,
    Float l)
{
#ifdef DEBUG
    for (int i = 0; i < n - 1; ++i)
    {
        CHECK_GT(lambda[i + 1], lambda[i]);
    }
#endif
    if (l <= lambda[0]) return vals[0];
    if (l >= lambda[n - 1]) return vals[n - 1];
    int offset = 0;
    common::math::FindInterval(n, [&](int index)
    {
        return lambda[index] <= l;
    });
#ifdef DEBUG
    CHECK(l >= lambda[offset] && l <= lambda[offset + 1]);
#endif
    Float t = (l - lambda[offset]) / (lambda[offset + 1] - lambda[offset]);
    return common::math::Lerp(t, vals[offset], vals[offset + 1]);
}


}
}