#include "VisibilityTester.h"
#include "../../common/math/Ray.h"
#include "../color/Spectrum.h"
#include "../scene/Scene.h"
#include "../primitive/Primitive.h"
#include "../interaction/SurfaceInteraction.h"

namespace core
{
namespace light
{


bool VisibilityTester::Unoccluded(const core::scene::Scene &scene) const
{
    return !scene.IntersectP(p0.SpawnRayTo(p1));
}

color::Spectrum VisibilityTester::Tr(const core::scene::Scene &scene, core::sampler::Sampler &sampler) const
{
    common::math::Rayf ray(p0.SpawnRayTo(p1));
    color::Spectrum Tr(FLOAT_1);
    while (true)
    {
        interaction::SurfaceInteraction isect;
        bool hitSurface = scene.Intersect(ray, &isect);
        // Handle opaque surface along ray's path
        if (hitSurface && isect.primitive->GetMaterial() != nullptr)
        {
            return color::Spectrum(FLOAT_0);
        }

        // Update transmittance for current ray segment
        if (ray.medium)
        {
            Tr *= ray.medium->Tr(ray, sampler);
        }

        // Generate next ray segment or return final transmittance
        if (!hitSurface)
        {
            break;
        }
        ray = isect.SpawnRayTo(p1);
    }

    return Tr;
}


}
}