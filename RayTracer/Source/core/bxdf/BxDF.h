#pragma once

#include "../../ForwardDeclaration.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Vec3.h"
#include "../../common/math/Constants.h"
#include "../color/Spectrum.h"

namespace core
{
namespace bxdf
{


enum BxDFType
{
    BSDF_REFLECTION = 1 << 0,
    BSDF_TRANSMISSION = 1 << 1,
    BSDF_DIFFUSE = 1 << 2,
    BSDF_GLOSSY = 1 << 3,
    BSDF_SPECULAR = 1 << 4,
    BSDF_ALL = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION,
};

class BxDF
{
public:

    const BxDFType type;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    BxDF(BxDFType type) : type(type)
    {}

    virtual ~BxDF()
    {}


    bool MatchesFlags(BxDFType t) const
    {
        return (type & t) == type;
    }

    virtual color::Spectrum f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const = 0;

    virtual color::Spectrum Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi,
        const common::math::Vec2f &sample, Float *pdf,
        BxDFType *sampledType = nullptr) const;

    virtual color::Spectrum rho(const common::math::Vec3f &wo, int nSamples,
        const common::math::Vec2f *samples) const;

    virtual color::Spectrum rho(int nSamples, const common::math::Vec2f *samples1,
        const common::math::Vec2f *samples2) const;

    virtual Float Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

};


////////////////////////////////////////////////////////////////////////////////
// BSDF Inline Functions
////////////////////////////////////////////////////////////////////////////////

inline
Float CosTheta(const common::math::Vec3f &w)
{
    return w.z;
}

inline
Float Cos2Theta(const common::math::Vec3f &w)
{
    return w.z * w.z;
}

inline
Float AbsCosTheta(const common::math::Vec3f &w)
{
    return std::abs(w.z);
}

inline
Float Sin2Theta(const common::math::Vec3f &w)
{
    return (std::max)(FLOAT_0, FLOAT_1 - Cos2Theta(w));
}

inline
Float SinTheta(const common::math::Vec3f &w)
{
    return std::sqrt(Sin2Theta(w));
}

inline
Float TanTheta(const common::math::Vec3f &w)
{
    return SinTheta(w) / CosTheta(w);
}

inline
Float Tan2Theta(const common::math::Vec3f &w)
{
    return Sin2Theta(w) / Cos2Theta(w);
}

inline
Float CosPhi(const common::math::Vec3f &w)
{
    Float sinTheta = SinTheta(w);
    return (sinTheta == FLOAT_0) ? FLOAT_1 : common::math::Clamp(w.x / sinTheta, -FLOAT_1, FLOAT_1);
}

inline
Float SinPhi(const common::math::Vec3f &w)
{
    Float sinTheta = SinTheta(w);
    return (sinTheta == FLOAT_0) ? FLOAT_0 : common::math::Clamp(w.y / sinTheta, -FLOAT_1, FLOAT_1);
}

inline
Float Cos2Phi(const common::math::Vec3f &w)
{
    return CosPhi(w) * CosPhi(w);
}

inline
Float Sin2Phi(const common::math::Vec3f &w)
{
    return SinPhi(w) * SinPhi(w);
}

inline
Float CosDPhi(const common::math::Vec3f &wa, const common::math::Vec3f &wb)
{
    return common::math::Clamp(
        (wa.x * wb.x + wa.y * wb.y) / std::sqrt((wa.x * wa.x + wa.y * wa.y) *
        (wb.x * wb.x + wb.y * wb.y)),
        -FLOAT_1, FLOAT_1);
}

inline
common::math::Vec3f Reflect(const common::math::Vec3f &wo, const common::math::Vec3f &n)
{
    return -wo + FLOAT_2 * Dot(wo, n) * n;
}

inline
bool Refract(const common::math::Vec3f &wi, const common::math::Vec3f &n, Float eta,
    common::math::Vec3f *wt)
{
    // Compute $\cos \theta_\roman{t}$ using Snell's law
    Float cosThetaI = Dot(n, wi);
    Float sin2ThetaI = (std::max)(FLOAT_0, FLOAT_1 - cosThetaI * cosThetaI);
    Float sin2ThetaT = eta * eta * sin2ThetaI;

    // Handle total internal reflection for transmission
    if (sin2ThetaT >= FLOAT_1) return false;
    Float cosThetaT = std::sqrt(FLOAT_1 - sin2ThetaT);
    *wt = eta * -wi + (eta * cosThetaI - cosThetaT) * common::math::Vec3f(n);
    return true;
}

inline
bool SameHemisphere(const common::math::Vec3f &w, const common::math::Vec3f &wp)
{
    return w.z * wp.z > FLOAT_0;
}


}
}