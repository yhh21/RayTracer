#include "Scene.h"
#include "../../common/math/Ray.h"
#include "../../common/math/Vec3.h"
#include "../interaction/SurfaceInteraction.h"
#include "../color/Spectrum.h"
#include "../primitive/Primitive.h"
#include "../light/Light.h"

namespace core
{
namespace scene
{


Scene::Scene(std::shared_ptr<core::primitive::Primitive> aggregate,
    const std::vector<std::shared_ptr<core::light::Light>> &lights)
    : lights(lights), aggregate(aggregate)
{
    // Scene Constructor Implementation
    worldBound = aggregate->WorldBound();
    for (const auto &light : lights)
    {
        light->Preprocess(*this);
        if (light->flags & (int)core::light::LightFlags::Infinite)
        {
            infiniteLights.push_back(light);
        }
    }
}

bool Scene::Intersect(const common::math::Rayf &ray, core::interaction::SurfaceInteraction *isect) const
{
    //++nIntersectionTests;
    CHECK_NE(ray.dir, common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0));
    return aggregate->Intersect(ray, isect);
}

bool Scene::IntersectP(const common::math::Rayf &ray) const
{
    //++nShadowTests;
    CHECK_NE(ray.dir, common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0));
    return aggregate->IntersectP(ray);
}

bool Scene::IntersectTr(common::math::Rayf ray, core::sampler::Sampler &sampler, core::interaction::SurfaceInteraction *isect,
    core::color::Spectrum *Tr) const
{
    *Tr = core::color::Spectrum(FLOAT_1);
    while (true)
    {
        bool hitSurface = Intersect(ray, isect);
        // Accumulate beam transmittance for ray segment
        if (ray.medium)
        {
            *Tr *= ray.medium->Tr(ray, sampler);
        }

        // Initialize next ray segment or terminate transmittance computation
        if (!hitSurface)
        {
            return false;
        }
        if (isect->primitive->GetMaterial() != nullptr)
        {
            return true;
        }
        ray = isect->SpawnRay(ray.dir);
    }
}


}
}