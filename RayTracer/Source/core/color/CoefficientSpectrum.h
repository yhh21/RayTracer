#pragma once

#include <vector>
#include "../../common/math/Constants.h"
#include "ColorExtern.h"

namespace core
{
namespace color
{


template<int SPECTRUM_SAMPLES_NUMBER>
class CoefficientSpectrum
{
public:
    static const int SAMPLE_NUMBER = SPECTRUM_SAMPLES_NUMBER;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    CoefficientSpectrum(Float v = FLOAT_0)
    {
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            c[i] = v;
        }
    }

    CoefficientSpectrum(const CoefficientSpectrum &s)
    {
        Clone(s);
    }

    CoefficientSpectrum &operator=(const CoefficientSpectrum &s)
    {
        Clone(s);
        return *this;
    }

    __forceinline
        void Clone(const CoefficientSpectrum &s)
    {
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            (*this)[i] = s[i];
        }
    }


    __forceinline
        const Float& operator [](const size_t index) const
    {
        CHECK_LT(index, SPECTRUM_SAMPLES_NUMBER);
        return c[index];
    }

    __forceinline
        Float& operator [](const size_t index)
    {
        CHECK_LT(index, SPECTRUM_SAMPLES_NUMBER);
        return c[index];
    }


    bool IsBlack() const
    {
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            if (FLOAT_0 != c[i])
            {
                return false;
            }
        }

        return true;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // Comparison Operators
    ////////////////////////////////////////////////////////////////////////////////

    bool operator==(const CoefficientSpectrum &s2) const
    {
        CoefficientSpectrum s1 = *this;
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            if (s1[i] != s2[i])
            {
                return false;
            }
        }

        return s1;
    }

    bool operator!=(const CoefficientSpectrum &s2) const
    {
        CoefficientSpectrum s1 = *this;
        return !(s1 == s2);
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Assignment Operators
    ////////////////////////////////////////////////////////////////////////////////

    CoefficientSpectrum operator+=(const CoefficientSpectrum &s2)
    {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            ret[i] += s2[i];
        }

        return ret;
    }

    CoefficientSpectrum operator*=(const CoefficientSpectrum &s2)
    {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            ret[i] *= s2[i];
        }

        return ret;
    }

    CoefficientSpectrum operator*=(const Float &a)
    {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            ret[i] *= a;
        }

        return ret;
    }

    CoefficientSpectrum operator/=(const Float &a)
    {
        CHECK_NE(a, FLOAT_0);

        CoefficientSpectrum ret = *this;
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            ret[i] /= a;
        }

        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Binary Operators
    ////////////////////////////////////////////////////////////////////////////////

    CoefficientSpectrum operator+(const CoefficientSpectrum &s2) const
    {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            ret[i] += s2[i];
        }

        return ret;
    }

    CoefficientSpectrum operator-(const CoefficientSpectrum &s2) const
    {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            ret[i] -= s2[i];
        }

        return ret;
    }

    CoefficientSpectrum operator/(const CoefficientSpectrum &s2) const
    {
        CHECK(!s2.IsBlack());

        CoefficientSpectrum ret = *this;
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            ret[i] /= s2[i];
        }

        return ret;
    }

    CoefficientSpectrum operator*(const CoefficientSpectrum &s2) const
    {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            ret[i] *= s2[i];
        }

        return ret;
    }

    CoefficientSpectrum operator*(const Float &a) const
    {
        CoefficientSpectrum ret = *this;
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            ret[i] *= a;
        }

        return ret;
    }

    friend inline
        CoefficientSpectrum operator*(const Float &a, const CoefficientSpectrum &s1)
    {
        return s1 * a;
    }

    CoefficientSpectrum operator/(const Float &a) const
    {
        CHECK_NE(a, FLOAT_0);

        CoefficientSpectrum ret = *this;
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            ret[i] /= a;
        }

        return ret;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Euclidian Space Operators
    ////////////////////////////////////////////////////////////////////////////////

    friend
        CoefficientSpectrum Sqrt(const CoefficientSpectrum &s)
    {
        CoefficientSpectrum ret;
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            ret[i] = std::sqrt(s[i]);
        }
        return ret;
    }

    CoefficientSpectrum Clamp(Float low = 0, Float high = (std::numeric_limits<Float>::max)()) const
    {
        CoefficientSpectrum ret;
        CoefficientSpectrum s = *this;
        for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
        {
            ret[i] = common::math::Clamp(s[i], low, high);
        }

        return ret;
    }


protected:
    Float c[SPECTRUM_SAMPLES_NUMBER];
};


}
}