#pragma once

#include <vector>
#include "ColorExtern.h"

using namespace Common::Math;

namespace Core
{
    namespace Color
    {
        template<int SPECTRUM_SAMPLES_NUMBER>
        class CoefficientSpectrum
        {
        public:
            static const int SAMPLE_NUMBER = SPECTRUM_SAMPLES_NUMBER;

            ////////////////////////////////////////////////////////////////////////////////
            /// Construction
            ////////////////////////////////////////////////////////////////////////////////

            CoefficientSpectrum(Float v = static_cast<Float>(0.0F))
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
                    c[i] = s.c[i];
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
                    if (static_cast<Float>(0.0F) != c[i])  return false;
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
                ret.c[i] = std::sqrt(s.c[i]);
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
                s.c[i] = Clamp(s.c[i], low, high);
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
                ret.c[i] = s1.c[i] + s2.c[i];
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
                ret.c[i] = s1.c[i] - s2.c[i];
            }

            return ret;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator/(const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s2)
        {
            CHECK_EQ(s2.IsBlack(), true);

            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> ret;
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                ret.c[i] = s1.c[i] / s2.c[i];
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
                ret.c[i] = s1.c[i] * s2.c[i];
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
                ret.c[i] = s1.c[i] * a;
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
            CHECK_NE(a, static_cast<Float>(0.0F));

            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> ret;
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                ret.c[i] = s1.c[i] / a;
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
                s1.c[i] += s2.c[i];
            }

            return s1;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator*=(CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s2)
        {
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                s1.c[i] *= s2.c[i];
            }

            return s1;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator*=(CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const Float &a)
        {
            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                s1.c[i] *= a;
            }

            return s1;
        }

        template<int SPECTRUM_SAMPLES_NUMBER> inline
            CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> operator/=(CoefficientSpectrum<SPECTRUM_SAMPLES_NUMBER> &s1
                , const Float &a)
        {
            CHECK_NE(a, static_cast<Float>(0.0F));

            for (int i = 0; i < SPECTRUM_SAMPLES_NUMBER; ++i)
            {
                s1.c[i] /= a;
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
                if (s1.c[i] != s2.c[i]) return false;
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