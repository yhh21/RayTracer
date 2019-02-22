#pragma once

#include "../../ForwardDeclaration.h"
#include "../color/Spectrum.h"

namespace core
{
namespace bxdf
{


class Fresnel
{
public:

    virtual ~Fresnel()
    {}

    virtual color::Spectrum Evaluate(Float cosI) const = 0;
};


class FresnelConductor : public Fresnel
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    FresnelConductor(const color::Spectrum &etaI, const color::Spectrum &etaT,
        const color::Spectrum &k)
        : etaI(etaI), etaT(etaT), k(k)
    {}


    color::Spectrum Evaluate(Float cosThetaI) const;

private:
    color::Spectrum etaI, etaT, k;
};


class FresnelDielectric : public Fresnel
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    FresnelDielectric(Float etaI, Float etaT) : etaI(etaI), etaT(etaT)
    {}


    color::Spectrum Evaluate(Float cosThetaI) const;

private:
    Float etaI, etaT;
};


class FresnelNoOp : public Fresnel
{
public:

    color::Spectrum Evaluate(Float) const;

};


////////////////////////////////////////////////////////////////////////////////
// Reflection Declarations
////////////////////////////////////////////////////////////////////////////////

Float FrDielectric(Float cosThetaI, Float etaI, Float etaT);

color::Spectrum FrConductor(Float cosThetaI, const color::Spectrum &etaI,
    const color::Spectrum &etaT, const color::Spectrum &k);

}
}