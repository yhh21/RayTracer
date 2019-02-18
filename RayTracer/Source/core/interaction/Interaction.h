#pragma once

#include "../../ForwardDeclaration.h"
#include "Medium.h"
#include "../../common/math/Vec3.h"
#include "../../common/math/Ray.h"

namespace core
{
namespace interaction
{


struct Interaction
{
    common::math::Vec3f p;
    Float time;
    common::math::Vec3f p_error;
    common::math::Vec3f wo;
    common::math::Vec3f n;
    interaction::MediumInterface medium_interface;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    Interaction() : time(FLOAT_0)
    {}

    Interaction(const common::math::Vec3f &p, const common::math::Vec3f &n, const common::math::Vec3f &p_error
        , const common::math::Vec3f &wo, Float time
        , const interaction::MediumInterface &medium_interface)
        : p(p), time(time), p_error(p_error), wo(Normalize(wo)), n(n)
        , medium_interface(medium_interface)
    {}

    Interaction(const common::math::Vec3f &p, const common::math::Vec3f &wo, Float time,
        const interaction::MediumInterface &medium_interface)
        : p(p), time(time), wo(wo), medium_interface(medium_interface)
    {}

    Interaction(const common::math::Vec3f &p, Float time,
        const interaction::MediumInterface &medium_interface)
        : p(p), time(time), medium_interface(medium_interface)
    {}


    bool IsSurfaceInteraction() const
    {
        return n != common::math::Vec3f(FLOAT_0);
    }

    common::math::Rayf SpawnRay(const common::math::Vec3f &d) const
    {
        common::math::Vec3f o = OffsetRayOrigin(p, p_error, n, d);
        return common::math::Rayf(o, d, (std::numeric_limits<Float>::max)(), FLOAT_0, time, GetMedium(d));
    }

    common::math::Rayf SpawnRayTo(const common::math::Vec3f &p2) const
    {
        common::math::Vec3f origin = OffsetRayOrigin(p, p_error, n, p2 - p);
        common::math::Vec3f d = p2 - p;
        return common::math::Rayf(origin, d, FLOAT_1 - common::math::SHADOW_EPSILON, FLOAT_0, time, GetMedium(d));
    }

    common::math::Rayf SpawnRayTo(const Interaction &it) const
    {
        common::math::Vec3f origin = OffsetRayOrigin(p, p_error, n, it.p - p);
        common::math::Vec3f target = OffsetRayOrigin(it.p, it.p_error, it.n, origin - it.p);
        common::math::Vec3f d = target - origin;
        return common::math::Rayf(origin, d, FLOAT_1 - common::math::SHADOW_EPSILON, FLOAT_0, time, GetMedium(d));
    }

    bool IsMediumInteraction() const
    {
        return !IsSurfaceInteraction();
    }

    const Medium *GetMedium(const common::math::Vec3f &w) const
    {
        return Dot(w, n) > 0 ? medium_interface.outside : medium_interface.inside;
    }

    const Medium *GetMedium() const
    {
        CHECK_EQ(medium_interface.inside, medium_interface.outside);
        return medium_interface.inside;
    }
};


}
}