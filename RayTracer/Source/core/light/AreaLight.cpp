#include "AreaLight.h"

namespace core
{
namespace light
{


AreaLight::AreaLight(const common::math::Transformf &LightToWorld, const core::interaction::MediumInterface &medium,
    int nSamples)
    : Light((int)LightFlags::Area, LightToWorld, medium, nSamples)
{

}


}
}