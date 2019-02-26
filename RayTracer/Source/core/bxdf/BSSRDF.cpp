#include "BSSRDF.h"
#include "../../common/math/Constants.h"

namespace core
{
namespace bxdf
{


Float FresnelMoment1(Float eta)
{
    Float eta2 = eta * eta,
        eta3 = eta2 * eta,
        eta4 = eta3 * eta,
        eta5 = eta4 * eta;
    if (eta < FLOAT_1)
    {
        return static_cast<Float>(0.45966F)
            - static_cast<Float>(1.73965F) * eta
            + static_cast<Float>(3.37668F) * eta2
            - static_cast<Float>(3.904945F) * eta3
            + static_cast<Float>(2.49277F) * eta4
            - static_cast<Float>(0.68441F) * eta5;
    }
    else
    {
        return -static_cast<Float>(4.61686F)
            + static_cast<Float>(11.1136F) * eta
            - static_cast<Float>(10.4646F) * eta2
            + static_cast<Float>(5.11455F) * eta3
            - static_cast<Float>(1.27198F) * eta4
            + static_cast<Float>(0.12746F) * eta5;
    }
}

Float FresnelMoment2(Float eta)
{
    Float eta2 = eta * eta,
        eta3 = eta2 * eta,
        eta4 = eta3 * eta,
        eta5 = eta4 * eta;
    if (eta < FLOAT_1)
    {
        return static_cast<Float>(0.27614F)
            - static_cast<Float>(0.87350F) * eta
            + static_cast<Float>(1.12077F) * eta2
            - static_cast<Float>(0.65095F) * eta3
            + static_cast<Float>(0.07883F) * eta4
            + static_cast<Float>(0.04860F) * eta5;
    }
    else
    {
        Float r_eta = FLOAT_1 / eta,
            r_eta2 = r_eta * r_eta,
            r_eta3 = r_eta2 * r_eta;

        return static_cast<Float>(-547.033F)
            + static_cast<Float>(45.3087F) * r_eta3
            - static_cast<Float>(218.725F) * r_eta2
            + static_cast<Float>(458.843F) * r_eta
            + static_cast<Float>(404.557F) * eta
            - static_cast<Float>(189.519F) * eta2
            + static_cast<Float>(54.9327F) * eta3
            - static_cast<Float>(9.00603F) * eta4
            + static_cast<Float>(0.63942F) * eta5;
    }
}


Float CatmullRom(int size, const Float *nodes, const Float *values, Float x)
{
    if (!(x >= nodes[0] && x <= nodes[size - 1]))
    {
        return FLOAT_0;
    }
    int idx = common::math::FindInterval(size, [&](int i)
    {
        return nodes[i] <= x;
    });

    Float x0 = nodes[idx],
        x1 = nodes[idx + 1];
    Float f0 = values[idx],
        f1 = values[idx + 1];
    Float width = x1 - x0;
    Float d0, d1;
    if (idx > 0)
    {
        d0 = width * (f1 - values[idx - 1]) / (x1 - nodes[idx - 1]);
    }
    else
    {
        d0 = f1 - f0;
    }

    if (idx + 2 < size)
    {
        d1 = width * (values[idx + 2] - f0) / (nodes[idx + 2] - x0);
    }
    else
    {
        d1 = f1 - f0;
    }

    Float t = (x - x0) / (x1 - x0),
        t2 = t * t,
        t3 = t2 * t;

    return (FLOAT_2 * t3 - FLOAT_3 * t2 + FLOAT_1) * f0
        + (-FLOAT_2 * t3 + FLOAT_3 * t2) * f1
        + (t3 - FLOAT_2 * t2 + t) * d0
        + (t3 - t2) * d1;
}

bool CatmullRomWeights(int size, const Float *nodes, Float x, int *offset, Float *weights)
{
    // Return _false_ if _x_ is out of bounds
    if (!(x >= nodes[0] && x <= nodes[size - 1]))
    {
        return false;
    }

    // Search for the interval _idx_ containing _x_
    int idx = common::math::FindInterval(size, [&](int i)
    {
        return nodes[i] <= x;
    });

    *offset = idx - 1;
    Float x0 = nodes[idx],
        x1 = nodes[idx + 1];

    // Compute the $t$ parameter and powers
    Float t = (x - x0) / (x1 - x0),
        t2 = t * t,
        t3 = t2 * t;

    // Compute initial node weights $w_1$ and $w_2$
    weights[1] = FLOAT_2 * t3 - FLOAT_3 * t2 + FLOAT_1;
    weights[2] = -FLOAT_2 * t3 + FLOAT_3 * t2;

    // Compute first node weight $w_0$
    if (idx > 0)
    {
        Float w0 = (t3 - FLOAT_2 * t2 + t) * (x1 - x0) / (x1 - nodes[idx - 1]);
        weights[0] = -w0;
        weights[2] += w0;
    }
    else
    {
        Float w0 = t3 - FLOAT_2 * t2 + t;
        weights[0] = FLOAT_0;
        weights[1] -= w0;
        weights[2] += w0;
    }

    // Compute last node weight $w_3$
    if (idx + 2 < size)
    {
        Float w3 = (t3 - t2) * (x1 - x0) / (nodes[idx + 2] - x0);
        weights[1] -= w3;
        weights[3] = w3;
    }
    else
    {
        Float w3 = t3 - t2;
        weights[1] -= w3;
        weights[2] += w3;
        weights[3] = FLOAT_0;
    }
    return true;
}

Float SampleCatmullRom(int n, const Float *x, const Float *f, const Float *F,
    Float u, Float *fval, Float *pdf)
{
    // Map _u_ to a spline interval by inverting _F_
    u *= F[n - 1];
    int i = common::math::FindInterval(n, [&](int i)
    {
        return F[i] <= u;
    });

    // Look up $x_i$ and function values of spline segment _i_
    Float x0 = x[i],
        x1 = x[i + 1];
    Float f0 = f[i],
        f1 = f[i + 1];
    Float width = x1 - x0;

    // Approximate derivatives using finite differences
    Float d0, d1;
    if (i > 0)
    {
        d0 = width * (f1 - f[i - 1]) / (x1 - x[i - 1]);
    }
    else
    {
        d0 = f1 - f0;
    }
    if (i + 2 < n)
    {
        d1 = width * (f[i + 2] - f0) / (x[i + 2] - x0);
    }
    else
    {
        d1 = f1 - f0;
    }

    // Re-scale _u_ for continous spline sampling step
    u = (u - F[i]) / width;

    // Invert definite integral over spline segment and return solution

    // Set initial guess for $t$ by importance sampling a linear interpolant
    Float t;
    if (f0 != f1)
    {
        t = (f0 - std::sqrt((std::max)(FLOAT_0, f0 * f0 + FLOAT_2 * u * (f1 - f0)))) /
            (f0 - f1);
    }
    else
    {
        t = u / f0;
    }

    Float a = FLOAT_0,
        b = FLOAT_1,
        Fhat,
        fhat;
    while (true)
    {
        // Fall back to a bisection step when _t_ is out of bounds
        if (!(t > a && t < b))
        {
            t = FLOAT_INV_2 * (a + b);
        }

        // Evaluate target function and its derivative in Horner form
        Fhat = t * (f0 + t * (FLOAT_INV_2 * d0 + t * ((FLOAT_1 / FLOAT_3) * (-FLOAT_2 * d0 - d1)
            + f1 - f0 + t * (static_cast<Float>(0.25F) * (d0 + d1) + FLOAT_INV_2 * (f0 - f1)))));
        fhat = f0 + t * (d0 + t * (-FLOAT_2 * d0 - d1 + FLOAT_3 * (f1 - f0) + t * (d0 + d1 + FLOAT_2 * (f0 - f1))));

        // Stop the iteration if converged
        if (std::abs(Fhat - u) < static_cast<Float>(1e-6f) || b - a < static_cast<Float>(1e-6f))
        {
            break;
        }

        // Update bisection bounds using updated _t_
        if (Fhat - u < FLOAT_0)
        {
            a = t;
        }
        else
        {
            b = t;
        }

        // Perform a Newton step
        t -= (Fhat - u) / fhat;
    }

    // Return the sample position and function value
    if (fval)
    {
        *fval = fhat;
    }
    if (pdf)
    {
        *pdf = fhat / F[n - 1];
    }
    return x0 + width * t;
}

Float SampleCatmullRom2D(int size1, int size2, const Float *nodes1,
    const Float *nodes2, const Float *values,
    const Float *cdf, Float alpha, Float u, Float *fval,
    Float *pdf)
{
    // Determine offset and coefficients for the _alpha_ parameter
    int offset;
    Float weights[4];
    if (!CatmullRomWeights(size1, nodes1, alpha, &offset, weights))
    {
        return FLOAT_0;
    }

    // Define a lambda function to interpolate table entries
    auto interpolate = [&](const Float *_array, int idx)
    {
        Float value = FLOAT_0;
        for (int i = 0; i < 4; ++i)
        {
            if (weights[i] != FLOAT_0)
            {
                value += _array[(offset + i) * size2 + idx] * weights[i];
            }
        }
        return value;
    };

    // Map _u_ to a spline interval by inverting the interpolated _cdf_
    Float maximum = interpolate(cdf, size2 - 1);
    u *= maximum;
    int idx = common::math::FindInterval(size2, [&](int i)
    {
        return interpolate(cdf, i) <= u;
    });

    // Look up node positions and interpolated function values
    Float f0 = interpolate(values, idx),
        f1 = interpolate(values, idx + 1);
    Float x0 = nodes2[idx],
        x1 = nodes2[idx + 1];
    Float width = x1 - x0;
    Float d0, d1;

    // Re-scale _u_ using the interpolated _cdf_
    u = (u - interpolate(cdf, idx)) / width;

    // Approximate derivatives using finite differences of the interpolant
    if (idx > 0)
    {
        d0 = width * (f1 - interpolate(values, idx - 1)) /
            (x1 - nodes2[idx - 1]);
    }
    else
    {
        d0 = f1 - f0;
    }
    if (idx + 2 < size2)
    {
        d1 = width * (interpolate(values, idx + 2) - f0) /
            (nodes2[idx + 2] - x0);
    }
    else
    {
        d1 = f1 - f0;
    }

    // Invert definite integral over spline segment and return solution

    // Set initial guess for $t$ by importance sampling a linear interpolant
    Float t;
    if (f0 != f1)
    {
        t = (f0 - std::sqrt((std::max)(FLOAT_0, f0 * f0 + FLOAT_2 * u * (f1 - f0)))) /
            (f0 - f1);
    }
    else
    {
        t = u / f0;
    }
    Float a = FLOAT_0, b = FLOAT_1, Fhat, fhat;
    while (true)
    {
        // Fall back to a bisection step when _t_ is out of bounds
        if (!(t >= a && t <= b))
        {
            t = FLOAT_INV_2 * (a + b);
        }

        // Evaluate target function and its derivative in Horner form
        Fhat = t * (f0 + t * (FLOAT_INV_2 * d0 + t * ((FLOAT_1 / FLOAT_3) * (-FLOAT_2 * d0 - d1)
            + f1 - f0 + t * (static_cast<Float>(0.25F) * (d0 + d1) + FLOAT_INV_2 * (f0 - f1)))));
        fhat = f0 + t * (d0 + t * (-FLOAT_2 * d0 - d1 + FLOAT_3 * (f1 - f0) + t * (d0 + d1 + FLOAT_2 * (f0 - f1))));

        // Stop the iteration if converged
        if (std::abs(Fhat - u) < static_cast<Float>(1e-6f) || b - a < static_cast<Float>(1e-6f))
        {
            break;
        }

        // Update bisection bounds using updated _t_
        if (Fhat - u < FLOAT_0)
        {
            a = t;
        }
        else
        {
            b = t;
        }

        // Perform a Newton step
        t -= (Fhat - u) / fhat;
    }

    // Return the sample position and function value
    if (fval)
    {
        *fval = fhat;
    }
    if (pdf)
    {
        *pdf = fhat / maximum;
    }
    return x0 + width * t;
}

Float IntegrateCatmullRom(int n, const Float *x, const Float *values, Float *cdf)
{
    Float sum = FLOAT_0;
    cdf[0] = FLOAT_0;
    for (int i = 0; i < n - 1; ++i)
    {
        // Look up $x_i$ and function values of spline segment _i_
        Float x0 = x[i],
            x1 = x[i + 1];
        Float f0 = values[i],
            f1 = values[i + 1];
        Float width = x1 - x0;

        // Approximate derivatives using finite differences
        Float d0, d1;
        if (i > 0)
        {
            d0 = width * (f1 - values[i - 1]) / (x1 - x[i - 1]);
        }
        else
        {
            d0 = f1 - f0;
        }
        if (i + 2 < n)
        {
            d1 = width * (values[i + 2] - f0) / (x[i + 2] - x0);
        }
        else
        {
            d1 = f1 - f0;
        }

        // Keep a running sum and build a cumulative distribution function
        sum += ((d0 - d1) * (FLOAT_1 /static_cast<Float>(12.0F)) + (f0 + f1) * FLOAT_INV_2) * width;
        cdf[i + 1] = sum;
    }
    return sum;
}

Float InvertCatmullRom(int n, const Float *x, const Float *values, Float u)
{
    // Stop when _u_ is out of bounds
    if (!(u > values[0]))
    {
        return x[0];
    }
    else if (!(u < values[n - 1]))
    {
        return x[n - 1];
    }

    // Map _u_ to a spline interval by inverting _values_
    int i = common::math::FindInterval(n, [&](int i)
    {
        return values[i] <= u;
    });

    // Look up $x_i$ and function values of spline segment _i_
    Float x0 = x[i],
        x1 = x[i + 1];
    Float f0 = values[i],
        f1 = values[i + 1];
    Float width = x1 - x0;

    // Approximate derivatives using finite differences
    Float d0, d1;
    if (i > 0)
    {
        d0 = width * (f1 - values[i - 1]) / (x1 - x[i - 1]);
    }
    else
    {
        d0 = f1 - f0;
    }
    if (i + 2 < n)
    {
        d1 = width * (values[i + 2] - f0) / (x[i + 2] - x0);
    }
    else
    {
        d1 = f1 - f0;
    }

    // Invert the spline interpolant using Newton-Bisection
    Float a = FLOAT_0, b = FLOAT_1, t = FLOAT_INV_2;
    Float Fhat, fhat;
    while (true)
    {
        // Fall back to a bisection step when _t_ is out of bounds
        if (!(t > a && t < b))
        {
            t = FLOAT_INV_2 * (a + b);
        }

        // Compute powers of _t_
        Float t2 = t * t, t3 = t2 * t;

        // Set _Fhat_ using Equation (8.27)
        Fhat = (FLOAT_2 * t3 - FLOAT_3 * t2 + FLOAT_1) * f0
            + (-FLOAT_2 * t3 + FLOAT_3 * t2) * f1
            + (t3 - FLOAT_2 * t2 + t) * d0
            + (t3 - t2) * d1;

        // Set _fhat_ using Equation (not present)
        fhat = (static_cast<Float>(6.0F) * t2
            - static_cast<Float>(6.0F) * t) * f0
            + (-static_cast<Float>(6.0F) * t2 + static_cast<Float>(6.0F) * t) * f1
            + (FLOAT_3 * t2 - static_cast<Float>(4.0F) * t + FLOAT_1) * d0
            + (FLOAT_3 * t2 - 2 * t) * d1;

        // Stop the iteration if converged
        if (std::abs(Fhat - u) < static_cast<Float>(1e-6f) || b - a < static_cast<Float>(1e-6f))
        {
            break;
        }

        // Update bisection bounds using updated _t_
        if (Fhat - u < FLOAT_0)
        {
            a = t;
        }
        else
        {
            b = t;
        }

        // Perform a Newton step
        t -= (Fhat - u) / fhat;
    }
    return x0 + t * width;
}



}
}