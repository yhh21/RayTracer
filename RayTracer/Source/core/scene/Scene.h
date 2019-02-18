#pragma once

#include "../../ForwardDeclaration.h"
#include "../../common/math/Bounds3.h"

namespace core
{
namespace scene
{


class Scene
{
public:

    std::vector<std::shared_ptr<core::light::Light>> lights;

    // Store infinite light sources separately for cases where we only want
    // to loop over them.
    std::vector<std::shared_ptr<core::light::Light>> infiniteLights;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    Scene(std::shared_ptr<core::primitive::Primitive> aggregate,
        const std::vector<std::shared_ptr<core::light::Light>> &lights);


    const common::math::Bounds3f &WorldBound() const
    {
        return worldBound;
    }

    bool Intersect(const common::math::Rayf &ray, core::interaction::SurfaceInteraction *isect) const;

    bool IntersectP(const common::math::Rayf &ray) const;

    bool IntersectTr(common::math::Rayf ray, core::sampler::Sampler &sampler, core::interaction::SurfaceInteraction *isect,
        core::color::Spectrum *transmittance) const;

private:

    std::shared_ptr<core::primitive::Primitive> aggregate;

    common::math::Bounds3f worldBound;
};


}
}