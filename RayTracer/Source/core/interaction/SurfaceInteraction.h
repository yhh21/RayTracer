#pragma once

#include "Interaction.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Vec3.h"
#include "../material/Material.h"

namespace core
{
namespace interaction
{


class SurfaceInteraction : public Interaction
{
public:

    common::math::Vec2f uv;
    common::math::Vec3f dp_du, dp_dv;
    common::math::Vec3f dn_du, dn_dv;
    const core::shape::Shape *shape = nullptr;
    struct
    {
        common::math::Vec3f n;
        common::math::Vec3f dp_du, dp_dv;
        common::math::Vec3f dn_du, dn_dv;
    } shading;

    const core::primitive::Primitive *primitive = nullptr;
    bxdf::BSDF *bsdf = nullptr;
    bxdf::BSSRDF *bssrdf = nullptr;

    mutable common::math::Vec3f dp_dx, dp_dy;
    mutable Float du_dx = FLOAT_0, dv_dx = FLOAT_0, du_dy = FLOAT_0, dv_dy = FLOAT_0;

    // Added after book publication. Shapes can optionally provide a face
    // index with an intersection point for use in Ptex texture lookups.
    // If Ptex isn't being used, then this value is ignored.
    int face_index = 0;

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    SurfaceInteraction()
    {}

    SurfaceInteraction(const common::math::Vec3f &p, const common::math::Vec3f &p_error, const common::math::Vec2f &uv
        , const common::math::Vec3f &wo, const common::math::Vec3f &dp_du, const common::math::Vec3f &dp_dv
        , const common::math::Vec3f &dn_du, const common::math::Vec3f &dn_dv, Float time, const shape::Shape *shape
        , int face_index = 0);


    void SetShadingGeometry(const common::math::Vec3f &dp_dus, const common::math::Vec3f &dp_dvs
        , const common::math::Vec3f &dn_dus, const common::math::Vec3f &dn_dvs
        , bool orientation_is_authoritative);

    void ComputeScatteringFunctions(
        const common::math::RayDifferentialf &ray, common::tool::MemoryArena &arena,
        bool allowMultipleLobes = false,
        material::TransportMode mode = material::TransportMode::Radiance);

    void ComputeDifferentials(const common::math::RayDifferentialf &r) const;

    color::Spectrum Le(const common::math::Vec3f &w) const;

};


}
}