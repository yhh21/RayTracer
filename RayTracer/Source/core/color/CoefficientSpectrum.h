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
            /// Construction
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
                    if (FLOAT_0 != c[i])  return false;
                }

                return true;
            }

        protected:
            Float c[SPECTRUM_SAMPLES_NUMBER];
        };


        ////////////////////////////////////////////////////////////////////////////////
        /// Euclidian Space Operators
        ////////////////////////////////////////////////////////////////////////////////

        template<int SPECTRUM_SAMPLES_NUMBER>
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> Sqrt(const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s)
        {
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> ret;
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                ret[i] = std::sqrt(s[i]);
            }
            return ret;
        }

        template<int SPECTRUM_SAMPLES_NUMBER>
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> Clamp(const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s,
            Float low = 0, Float high = (std::numeric_limits<Float>::max)())
        {
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> ret;
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                ret[i] = common::math::Clamp(s[i], low, high);
            }

            return ret;
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// Binary Operators
        ////////////////////////////////////////////////////////////////////////////////

            template<int SPECTRUM_SAMPLES_NUMBER>
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator+(const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s2)
        {
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> ret;
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                ret[i] = s1[i] + s2[i];
            }

            return ret;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator-(const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s2)
        {
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> ret;
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                ret[i] = s1[i] - s2[i];
            }

            return ret;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator/(const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s2)
        {
            CHECK(!s2.IsBlack());

            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> ret;
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                ret[i] = s1[i] / s2[i];
            }

            return ret;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator*(const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s2)
        {
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> ret;
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                ret[i] = s1[i] * s2[i];
            }

            return ret;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator*(const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const Float &a)
        {
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> ret;
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                ret[i] = s1[i] * a;
            }

            return ret;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator*(const Float &a
                , const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1)
        {
            return s1 * a;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator/(const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const Float &a)
        {
            CHECK_NE(a, FLOAT_0);

            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> ret;
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                ret[i] = s1[i] / a;
            }

            return ret;
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// Assignment Operators
        ////////////////////////////////////////////////////////////////////////////////

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator+=(CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s2)
        {
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                s1[i] += s2[i];
            }

            return s1;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator*=(CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s2)
        {
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                s1[i] *= s2[i];
            }

            return s1;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator*=(CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const Float &a)
        {
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                s1[i] *= a;
            }

            return s1;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator/=(CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const Float &a)
        {
            CHECK_NE(a, FLOAT_0);

            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                s1[i] /= a;
            }

            return s1;
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// Comparison Operators
        ////////////////////////////////////////////////////////////////////////////////

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            bool operator==(CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s2)
        {
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                if (s1[i] != s2[i]) return false;
            }

            return s1;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            bool operator!=(CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s2)
        {
            return !(s1 == s2);
        }
    }
}