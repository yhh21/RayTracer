#include "SurfaceInteraction.h"
#include "../shape/Shape.h"
#include "../primitive/Primitive.h"
#include "../../common/math/RayDifferential.h"
#include "../../common/math/Transform.h"
#include "../color/Spectrum.h"
#include "../light/AreaLight.h"

namespace core
{
namespace interaction
{


SurfaceInteraction::SurfaceInteraction(const common::math::Vec3f &p, const common::math::Vec3f &p_error
    , const common::math::Vec2f &uv
    , const common::math::Vec3f &wo, const common::math::Vec3f &dp_du, const common::math::Vec3f &dp_dv
    , const common::math::Vec3f &dn_du, const common::math::Vec3f &dn_dv, Float time, const core::shape::Shape *shape
    , int face_index)
    : Interaction(p, common::math::Vec3f(Normalize(Cross(dp_du, dp_dv))), p_error, wo, time, nullptr)
    , uv(uv), dp_du(dp_du), dp_dv(dp_dv), dn_du(dn_du), dn_dv(dn_dv)
    , shape(shape), face_index(face_index)
{
    // Initialize shading geometry from true geometry
    shading.n = n;
    shading.dp_du = dp_du;
    shading.dp_dv = dp_dv;
    shading.dn_du = dn_du;
    shading.dn_dv = dn_dv;

    // Adjust normal based on orientation and handedness
    if (nullptr != shape && (shape->reverse_orientation ^ shape->transform_swaps_handedness))
    {
        n *= -FLOAT_1;
        shading.n *= -FLOAT_1;
    }
}

void SurfaceInteraction::SetShadingGeometry(const common::math::Vec3f &dp_dus, const common::math::Vec3f &dp_dvs
    , const common::math::Vec3f &dn_dus, const common::math::Vec3f &dn_dvs
    , bool orientation_is_authoritative)
{
    // Compute _shading.n_ for _SurfaceInteraction_
    shading.n = Normalize(Cross(dp_dus, dp_dvs));
    if (shape && (shape->reverse_orientation ^ shape->transform_swaps_handedness))
    {
        shading.n = -shading.n;
    }

    if (orientation_is_authoritative)
    {
        n = Faceforward(n, shading.n);
    }
    else
    {
        shading.n = Faceforward(shading.n, n);
    }

    // Initialize _shading_ partial derivative values
    shading.dp_du = dp_dus;
    shading.dp_dv = dp_dvs;
    shading.dn_du = dn_dus;
    shading.dn_dv = dn_dvs;
}

void SurfaceInteraction::ComputeScatteringFunctions(const common::math::RayDifferentialf &ray,
    common::tool::MemoryArena &arena,
    bool allowMultipleLobes,
    material::TransportMode mode)
{
    ComputeDifferentials(ray);
    this->primitive->ComputeScatteringFunctions(this, arena, mode,
        allowMultipleLobes);
}

void SurfaceInteraction::ComputeDifferentials(
    const common::math::RayDifferentialf &ray) const
{
    if (ray.has_differentials)
    {
        // Estimate screen space change in $\pt{}$ and $(u,v)$

        // Compute auxiliary intersection points with plane
        Float d = Dot(n, common::math::Vec3f(p.x, p.y, p.z));
        Float tx = -(Dot(n, common::math::Vec3f(ray.rx_origin)) - d) / Dot(n, ray.rx_direction);
        if (std::isinf(tx) || std::isnan(tx))
        {
            goto GOTO_FAIL;
        }
        common::math::Vec3f px = ray.rx_origin + tx * ray.rx_direction;
        Float ty = -(Dot(n, common::math::Vec3f(ray.ry_origin)) - d) / Dot(n, ray.ry_direction);
        if (std::isinf(ty) || std::isnan(ty))
        {
            goto GOTO_FAIL;
        }
        common::math::Vec3f py = ray.ry_origin + ty * ray.ry_direction;
        dp_dx = px - p;
        dp_dy = py - p;

        // Compute $(u,v)$ offsets at auxiliary points

        // Choose two dimensions to use for ray offset computation
        int dim[2];
        if (std::abs(n.x) > std::abs(n.y) && std::abs(n.x) > std::abs(n.z))
        {
            dim[0] = 1;
            dim[1] = 2;
        }
        else if (std::abs(n.y) > std::abs(n.z))
        {
            dim[0] = 0;
            dim[1] = 2;
        }
        else
        {
            dim[0] = 0;
            dim[1] = 1;
        }

        // Initialize _A_, _Bx_, and _By_ matrices for offset computation
        Float A[2][2] = {{dp_du[dim[0]], dp_dv[dim[0]]}, {dp_du[dim[1]], dp_dv[dim[1]]}};
        Float Bx[2] = {px[dim[0]] - p[dim[0]], px[dim[1]] - p[dim[1]]};
        Float By[2] = {py[dim[0]] - p[dim[0]], py[dim[1]] - p[dim[1]]};
        if (!common::math::SolveLinearSystem2x2(A, Bx, &du_dx, &dv_dx))
        {
            du_dx = dv_dx = FLOAT_0;
        }
        if (!common::math::SolveLinearSystem2x2(A, By, &du_dy, &dv_dy))
        {
            du_dy = dv_dy = FLOAT_0;
        }
    }
    else
    {
    GOTO_FAIL:
        du_dx = dv_dx = FLOAT_0;
        du_dy = dv_dy = FLOAT_0;
        dp_dx = dp_dy = common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_0);
    }
}

color::Spectrum SurfaceInteraction::Le(const common::math::Vec3f &w) const
{
    const light::AreaLight *area = primitive->GetAreaLight();
    return area ? area->L(*this, w) : color::Spectrum(FLOAT_0);
}

}
}