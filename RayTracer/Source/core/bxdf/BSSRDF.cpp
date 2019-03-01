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


}
}