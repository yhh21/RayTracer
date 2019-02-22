#include "Interpolation.h"
#include "../../common/math/Constants.h"

namespace core
{
namespace bxdf
{

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
    Float x0 = nodes[idx], x1 = nodes[idx + 1];
    Float f0 = values[idx], f1 = values[idx + 1];
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

    Float t = (x - x0) / (x1 - x0), t2 = t * t, t3 = t2 * t;
    return (FLOAT_2 * t3 - FLOAT_3 * t2 + FLOAT_1) * f0 + (-FLOAT_2 * t3 + FLOAT_3 * t2) * f1 +
        (t3 - FLOAT_2 * t2 + t) * d0 + (t3 - t2) * d1;
}

bool CatmullRomWeights(int size, const Float *nodes, Float x, int *offset,
    Float *weights)
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
    Float x0 = nodes[idx], x1 = nodes[idx + 1];

    // Compute the $t$ parameter and powers
    Float t = (x - x0) / (x1 - x0), t2 = t * t, t3 = t2 * t;

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
    Float x0 = x[i], x1 = x[i + 1];
    Float f0 = f[i], f1 = f[i + 1];
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
        t = (f0 - std::sqrt((std::max)(FLOAT_0, f0 * f0 + FLOAT_2 * u * (f1 - f0)))) / (f0 - f1);
    }
    else
    {
        t = u / f0;
    }
    Float a = FLOAT_0, b = FLOAT_1, Fhat, fhat;
    while (true)
    {
        // Fall back to a bisection step when _t_ is out of bounds
        if (!(t > a && t < b))
        {
            t = FLOAT_INV_2 * (a + b);
        }

        // Evaluate target function and its derivative in Horner form
        Fhat = t * (f0 + t * (FLOAT_INV_2 * d0
            + t * ((FLOAT_1 / FLOAT_3) * (-FLOAT_2 * d0 - d1) + f1 - f0 +
                    t * (FLOAT_INV_2 * FLOAT_INV_2 * (d0 + d1) + FLOAT_INV_2 * (f0 - f1)))));
        fhat = f0 + t * (d0 + t * (-FLOAT_2 * d0
            - d1 + FLOAT_3 * (f1 - f0) +
                    t * (d0 + d1 + FLOAT_2 * (f0 - f1))));

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
    auto interpolate = [&](const Float *array, int idx)
    {
        Float value = 0;
        for (int i = 0; i < 4; ++i)
        {
            if (weights[i] != FLOAT_0)
            {
                value += array[(offset + i) * size2 + idx] * weights[i];
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
    Float f0 = interpolate(values, idx), f1 = interpolate(values, idx + 1);
    Float x0 = nodes2[idx], x1 = nodes2[idx + 1];
    Float width = x1 - x0;
    Float d0, d1;

    // Re-scale _u_ using the interpolated _cdf_
    u = (u - interpolate(cdf, idx)) / width;

    // Approximate derivatives using finite differences of the interpolant
    if (idx > 0)
    {
        d0 = width * (f1 - interpolate(values, idx - 1)) / (x1 - nodes2[idx - 1]);
    }
    else
    {
        d0 = f1 - f0;
    }
    if (idx + 2 < size2)
    {
        d1 = width * (interpolate(values, idx + 2) - f0) / (nodes2[idx + 2] - x0);
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
        t = (f0 - std::sqrt((std::max)(FLOAT_0, f0 * f0 + FLOAT_2 * u * (f1 - f0)))) / (f0 - f1);
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
        Fhat = t * (f0 + t * (FLOAT_INV_2 * d0
            + t * ((FLOAT_1 / FLOAT_3) * (-FLOAT_2 * d0 - d1) + f1 - f0 +
                    t * (FLOAT_INV_2 * FLOAT_INV_2 * (d0 + d1) + FLOAT_INV_2 * (f0 - f1)))));
        fhat = f0 + t * (d0 + t * (-FLOAT_2 * d0
            - d1 + FLOAT_3 * (f1 - f0) +
                    t * (d0 + d1 + FLOAT_2 * (f0 - f1))));

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

Float IntegrateCatmullRom(int n, const Float *x, const Float *values,
    Float *cdf)
{
    Float sum = FLOAT_0;
    cdf[0] = FLOAT_0;
    for (int i = 0; i < n - 1; ++i)
    {
        // Look up $x_i$ and function values of spline segment _i_
        Float x0 = x[i], x1 = x[i + 1];
        Float f0 = values[i], f1 = values[i + 1];
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
        sum += ((d0 - d1) * (FLOAT_1 / static_cast<Float>(12.0F)) + (f0 + f1) * FLOAT_INV_2) * width;
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
    Float x0 = x[i], x1 = x[i + 1];
    Float f0 = values[i], f1 = values[i + 1];
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
        Fhat = (FLOAT_2 * t3 - FLOAT_3 * t2 + FLOAT_1) * f0 + (-FLOAT_2 * t3 + FLOAT_3 * t2) * f1
            + (t3 - FLOAT_2 * t2 + t) * d0 + (t3 - t2) * d1;

        // Set _fhat_ using Equation (not present)
        fhat = (FLOAT_2 * FLOAT_3 * t2 - FLOAT_2 * FLOAT_3 * t) * f0
            + (-FLOAT_2 * FLOAT_3 * t2 + FLOAT_2 * FLOAT_3 * t) * f1 +
            (FLOAT_2 * t2 - FLOAT_2 * FLOAT_2 * t + FLOAT_1) * d0 + (FLOAT_3 * t2 - FLOAT_2 * t) * d1;

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

    return value;
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
    return f;
}


}
}