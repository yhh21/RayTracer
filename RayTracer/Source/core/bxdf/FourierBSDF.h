#pragma once

#include "BxDF.h"
#include "../material/Material.h"

namespace core
{
namespace bxdf
{


struct FourierBSDFTable
{
    Float eta;
    int mMax;
    int nChannels;
    int nMu;
    Float *mu;
    int *m;
    int *aOffset;
    Float *a;
    Float *a0;
    Float *cdf;
    Float *recip;

    /* TODO
    static bool Read(const std::string &filename, FourierBSDFTable *table);
    */

    const Float *GetAk(int offsetI, int offsetO, int *mptr) const
    {
        *mptr = m[offsetO * nMu + offsetI];
        return a + aOffset[offsetO * nMu + offsetI];
    }

    bool GetWeightsAndOffset(Float cosTheta, int *offset, Float weights[4]) const;
};


class FourierBSDF : public BxDF
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    FourierBSDF(const FourierBSDFTable &bsdfTable, core::material::TransportMode mode)
        : BxDF(BxDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_GLOSSY)),
        bsdfTable(bsdfTable),
        mode(mode)
    {}


    core::color::Spectrum f(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

    core::color::Spectrum Sample_f(const common::math::Vec3f &wo, common::math::Vec3f *wi
        , const common::math::Vec2f &u, Float *pdf, BxDFType *sampledType) const;

    Float Pdf(const common::math::Vec3f &wo, const common::math::Vec3f &wi) const;

private:

    const FourierBSDFTable &bsdfTable;

    const core::material::TransportMode mode;
};


Float Fourier(const Float *a, int m, double cosPhi);

Float SampleFourier(const Float *ak, const Float *recip, int m, Float u, Float *pdf, Float *phiPtr);


}
}