#include "BSSRDFTable.h"

namespace core
{
namespace bxdf
{


BSSRDFTable::BSSRDFTable(int nRhoSamples, int nRadiusSamples)
    : nRhoSamples(nRhoSamples),
    nRadiusSamples(nRadiusSamples),
    rhoSamples(new Float[nRhoSamples]),
    radiusSamples(new Float[nRadiusSamples]),
    profile(new Float[nRadiusSamples * nRhoSamples]),
    rhoEff(new Float[nRhoSamples]),
    profileCDF(new Float[nRadiusSamples * nRhoSamples])
{}


}
}