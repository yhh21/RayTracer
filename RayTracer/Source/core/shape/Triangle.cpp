#include "Triangle.h"

#include "../../common/math/Bounds3.h"
#include "../../common/math/Ray.h"
#include "../../common/math/Transform.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Vec3.h"
#include "../interaction/MediumInteraction.h"
#include "../interaction/SurfaceInteraction.h"
#include "../sampler/Sampling.h"
#include <array>

namespace core
{
namespace shape
{


Float Triangle::Area() const
{
    // Get triangle vertices in _p0_, _p1_, and _p2_
    const common::math::Vec3f &p0 = mesh->p[v[0]];
    const common::math::Vec3f &p1 = mesh->p[v[1]];
    const common::math::Vec3f &p2 = mesh->p[v[2]];

    return FLOAT_INV_2 * Length(Cross(p1 - p0, p2 - p0));
}

common::math::Bounds3f Triangle::ObjectBound() const
{
    // Get triangle vertices in _p0_, _p1_, and _p2_
    const common::math::Vec3f &p0 = mesh->p[v[0]];
    const common::math::Vec3f &p1 = mesh->p[v[1]];
    const common::math::Vec3f &p2 = mesh->p[v[2]];

    return Union(common::math::Bounds3f((*world_to_object)(p0), (*world_to_object)(p1))
        , (*world_to_object)(p2));
}

common::math::Bounds3f Triangle::WorldBound() const
{
    // Get triangle vertices in _p0_, _p1_, and _p2_
    const common::math::Vec3f &p0 = mesh->p[v[0]];
    const common::math::Vec3f &p1 = mesh->p[v[1]];
    const common::math::Vec3f &p2 = mesh->p[v[2]];

    return Union(common::math::Bounds3f(p0, p1), p2);
}


bool Triangle::Intersect(const common::math::Rayf &ray, Float *t_hit, interaction::SurfaceInteraction *isect
    , bool test_alpha_texture) const
{
    /*
    ProfilePhase p(Prof::TriIntersect);
    ++nTests;
    */
    // Get triangle vertices in _p0_, _p1_, and _p2_
    const common::math::Vec3f &p0 = mesh->p[v[0]];
    const common::math::Vec3f &p1 = mesh->p[v[1]];
    const common::math::Vec3f &p2 = mesh->p[v[2]];

    // Perform ray--triangle intersection test

    // common::math::Transformf triangle vertices to ray coordinate space

    // Translate vertices based on ray origin
    common::math::Vec3f p0t = p0 - common::math::Vec3f(ray.origin);
    common::math::Vec3f p1t = p1 - common::math::Vec3f(ray.origin);
    common::math::Vec3f p2t = p2 - common::math::Vec3f(ray.origin);

    // Permute components of triangle vertices and ray direction
    int kz = MaxDim(Abs(ray.dir));
    int kx = (kz + 1) % 3;
    int ky = (kx + 1) % 3;

    common::math::Vec3f d = Permute(ray.dir, kx, ky, kz);
    p0t = Permute(p0t, kx, ky, kz);
    p1t = Permute(p1t, kx, ky, kz);
    p2t = Permute(p2t, kx, ky, kz);

    // Apply shear transformation to translated vertex positions
    Float s_x = -d.x / d.z;
    Float s_y = -d.y / d.z;
    Float s_z = FLOAT_1 / d.z;

    p0t.x += s_x * p0t.z;
    p0t.y += s_y * p0t.z;
    p1t.x += s_x * p1t.z;
    p1t.y += s_y * p1t.z;
    p2t.x += s_x * p2t.z;
    p2t.y += s_y * p2t.z;

    // Compute edge function coefficients _e0_, _e1_, and _e2_
    Float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
    Float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
    Float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

    // Fall back to double precision test at triangle edges
    if (sizeof(Float) == sizeof(float)
        && (FLOAT_0 == e0 || FLOAT_0 == e1 || FLOAT_0 == e2))
    {
        double p2t_x_p1t_y = static_cast<double>(p2t.x) * static_cast<double>(p1t.y);
        double p2t_y_p1t_x = static_cast<double>(p2t.y) * static_cast<double>(p1t.x);
        e0 = (float)(p2t_y_p1t_x - p2t_x_p1t_y);

        double p0t_x_p2t_y = static_cast<double>(p0t.x) * static_cast<double>(p2t.y);
        double p0t_y_p2t_x = static_cast<double>(p0t.y) * static_cast<double>(p2t.x);
        e1 = (float)(p0t_y_p2t_x - p0t_x_p2t_y);

        double p1t_x_p0t_y = static_cast<double>(p1t.x) * static_cast<double>(p0t.y);
        double p1t_y_p0t_x = static_cast<double>(p1t.y) * static_cast<double>(p0t.x);
        e2 = (float)(p1t_y_p0t_x - p1t_x_p0t_y);
    }

    // Perform triangle edge and determinant tests
    if ((e0 < FLOAT_0 || e1 < FLOAT_0 || e2 < FLOAT_0)
        && (e0 > FLOAT_0 || e1 > FLOAT_0 || e2 > FLOAT_0))
    {
        return false;
    }

    Float det = e0 + e1 + e2;
    if (FLOAT_0 == det)
    {
        return false;
    }

    // Compute scaled hit distance to triangle and test against ray $t$ range
    p0t.z *= s_z;
    p1t.z *= s_z;
    p2t.z *= s_z;
    Float t_scaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
    if (det < FLOAT_0 && (t_scaled >= ray.t_min || t_scaled < ray.t_max * det))
    {
        return false;
    }
    else if (det > FLOAT_0 && (t_scaled <= ray.t_min || t_scaled > ray.t_max * det))
    {
        return false;
    }

    // Compute barycentric coordinates and $t$ value for triangle intersection
    Float inv_det = FLOAT_1 / det;
    Float b0 = e0 * inv_det;
    Float b1 = e1 * inv_det;
    Float b2 = e2 * inv_det;
    Float t = t_scaled * inv_det;

    // Ensure that computed triangle $t$ is conservatively greater than zero

    // Compute $\delta_z$ term for triangle $t$ error bounds
    Float max_Zt = MaxComponent(Abs(common::math::Vec3f(p0t.z, p1t.z, p2t.z)));
    Float delta_Z = common::math::Gamma(3) * max_Zt;

    // Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
    Float max_Xt = MaxComponent(Abs(common::math::Vec3f(p0t.x, p1t.x, p2t.x)));
    Float delta_X = common::math::Gamma(5) * (max_Xt + max_Zt);

    Float max_Yt = MaxComponent(Abs(common::math::Vec3f(p0t.y, p1t.y, p2t.y)));
    Float delta_Y = common::math::Gamma(5) * (max_Yt + max_Zt);

    // Compute $\delta_e$ term for triangle $t$ error bounds
    Float delta_E = FLOAT_2 * (common::math::Gamma(2) * max_Xt * max_Yt + delta_Y * max_Xt + delta_X * max_Yt);

    // Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
    Float max_E = MaxComponent(Abs(common::math::Vec3f(e0, e1, e2)));
    Float delta_T = FLOAT_3 * (common::math::Gamma(3) * max_E * max_Zt + delta_E * max_Zt
        + delta_Z * max_E) * std::abs(inv_det);
    if (t <= delta_T) return false;

    // Compute triangle partial derivatives
    common::math::Vec3f dp_du, dp_dv;
    common::math::Vec2f uv[3];
    GetUVs(uv);

    // Compute deltas for triangle partial derivatives
    common::math::Vec2f duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
    common::math::Vec3f dp02 = p0 - p2, dp12 = p1 - p2;
    Float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
    bool degenerate_UV = std::abs(determinant) < static_cast<Float>(1e-8);
    if (!degenerate_UV)
    {
        Float inv_det = FLOAT_1 / determinant;
        dp_du = (duv12[1] * dp02 - duv02[1] * dp12) * inv_det;
        dp_dv = (-duv12[0] * dp02 + duv02[0] * dp12) * inv_det;
    }

    if (degenerate_UV || FLOAT_0 == LengthSquared(Cross(dp_du, dp_dv)))
    {
        // Handle zero determinant for triangle partial derivative matrix
        common::math::Vec3f ng = Cross(p2 - p0, p1 - p0);

        // The triangle is actually degenerate; the intersection is bogus.
        if (FLOAT_0 == LengthSquared(ng))
        {
            return false;
        }

        CoordinateSystem(Normalize(ng), &dp_du, &dp_dv);
    }

    // Compute error bounds for triangle intersection
    Float x_abs_sum = (std::abs(b0 * p0.x) + std::abs(b1 * p1.x) + std::abs(b2 * p2.x));
    Float y_abs_sum = (std::abs(b0 * p0.y) + std::abs(b1 * p1.y) + std::abs(b2 * p2.y));
    Float z_abs_sum = (std::abs(b0 * p0.z) + std::abs(b1 * p1.z) + std::abs(b2 * p2.z));
    common::math::Vec3f p_error = common::math::Gamma(7) * common::math::Vec3f(x_abs_sum, y_abs_sum, z_abs_sum);

    // Interpolate $(u,v)$ parametric coordinates and hit point
    common::math::Vec3f p_hit = b0 * p0 + b1 * p1 + b2 * p2;
    common::math::Vec2f uv_hit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];

    // Test intersection against alpha texture, if present
    if (test_alpha_texture && nullptr != mesh->alpha_mask)
    {
        interaction::SurfaceInteraction isect_local(p_hit, common::math::Vec3f(FLOAT_0), uv_hit
            , -ray.dir, dp_du, dp_dv, common::math::Vec3f(FLOAT_0), common::math::Vec3f(FLOAT_0)
            , ray.time, this);

        if (FLOAT_0 == mesh->alpha_mask->Evaluate(isect_local))
        {
            return false;
        }
    }

    // Fill in _SurfaceInteraction_ from triangle hit
    *isect = interaction::SurfaceInteraction(p_hit, p_error, uv_hit
        , -ray.dir, dp_du, dp_dv, common::math::Vec3f(FLOAT_0), common::math::Vec3f(FLOAT_0)
        , ray.time, this, face_index);

    // Override surface normal in _isect_ for triangle
    isect->n = isect->shading.n = common::math::Vec3f(Normalize(Cross(dp02, dp12)));
    if (nullptr != mesh->n || nullptr != mesh->s)
    {
        // Initialize _Triangle_ shading geometry

        // Compute shading normal _ns_ for triangle
        common::math::Vec3f ns;
        if (nullptr != mesh->n)
        {
            ns = (b0 * mesh->n[v[0]] + b1 * mesh->n[v[1]] + b2 * mesh->n[v[2]]);
            if (LengthSquared(ns) > FLOAT_0)
            {
                ns = Normalize(ns);
            }
            else
            {
                ns = isect->n;
            }
        }
        else
        {
            ns = isect->n;
        }

        // Compute shading tangent _ss_ for triangle
        common::math::Vec3f ss;
        if (nullptr != mesh->s)
        {
            ss = (b0 * mesh->s[v[0]] + b1 * mesh->s[v[1]] + b2 * mesh->s[v[2]]);
            if (LengthSquared(ss) > FLOAT_0)
            {
                ss = Normalize(ss);
            }
            else
            {
                ss = Normalize(isect->dp_du);
            }
        }
        else
        {
            ss = Normalize(isect->dp_du);
        }

        // Compute shading bitangent _ts_ for triangle and adjust _ss_
        common::math::Vec3f ts = Cross(ss, ns);
        if (LengthSquared(ts) > FLOAT_0)
        {
            ts = Normalize(ts);
            ss = Cross(ts, ns);
        }
        else
        {
            CoordinateSystem(ns, &ss, &ts);
        }

        // Compute $\dn_du$ and $\dn_dv$ for triangle shading geometry
        common::math::Vec3f dn_du, dn_dv;
        if (nullptr != mesh->n)
        {
            // Compute deltas for triangle partial derivatives of normal
            common::math::Vec2f duv02 = uv[0] - uv[2];
            common::math::Vec2f duv12 = uv[1] - uv[2];
            common::math::Vec3f dn1 = mesh->n[v[0]] - mesh->n[v[2]];
            common::math::Vec3f dn2 = mesh->n[v[1]] - mesh->n[v[2]];
            Float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
            bool degenerate_UV = std::abs(determinant) < static_cast<Float>(1e-8);
            if (degenerate_UV)
            {
                // We can still compute dn_du and dn_dv, with respect to the
                // same arbitrary coordinate s_ystem we use to compute dp_du
                // and dp_dv when this happens. It's important to do this
                // (rather than giving up) so that ray differentials for
                // rays reflected from triangles with degenerate
                // parameterizations are still reasonable.
                common::math::Vec3f dn = Cross(common::math::Vec3f(mesh->n[v[2]] - mesh->n[v[0]]),
                    common::math::Vec3f(mesh->n[v[1]] - mesh->n[v[0]]));
                if (LengthSquared(dn) == FLOAT_0)
                {
                    dn_du = dn_dv = common::math::Vec3f(FLOAT_0);
                }
                else
                {
                    common::math::Vec3f dnu, dnv;
                    CoordinateSystem(dn, &dnu, &dnv);
                    dn_du = common::math::Vec3f(dnu);
                    dn_dv = common::math::Vec3f(dnv);
                }
            }
            else
            {
                Float inv_det = FLOAT_1 / determinant;
                dn_du = (duv12[1] * dn1 - duv02[1] * dn2) * inv_det;
                dn_dv = (-duv12[0] * dn1 + duv02[0] * dn2) * inv_det;
            }
        }
        else
        {
            dn_du = dn_dv = common::math::Vec3f(FLOAT_0);
        }

        isect->SetShadingGeometry(ss, ts, dn_du, dn_dv, true);
    }

    // Ensure correct orientation of the geometric normal
    if (nullptr != mesh->n)
    {
        isect->n = Faceforward(isect->n, isect->shading.n);
    }
    else if (reverse_orientation ^ transform_swaps_handedness)
    {
        isect->n = isect->shading.n = -isect->n;
    }
    *t_hit = t;
    //++nHits;
    return true;
}

bool Triangle::IntersectP(const common::math::Rayf &ray, bool test_alpha_texture) const
{
    /*
    ProfilePhase p(Prof::TriIntersectP);
    ++nTests;
    */
    // Get triangle vertices in _p0_, _p1_, and _p2_
    const common::math::Vec3f &p0 = mesh->p[v[0]];
    const common::math::Vec3f &p1 = mesh->p[v[1]];
    const common::math::Vec3f &p2 = mesh->p[v[2]];

    // Perform ray--triangle intersection test

    // common::math::Transformf triangle vertices to ray coordinate space

    // Translate vertices based on ray origin
    common::math::Vec3f p0t = p0 - common::math::Vec3f(ray.origin);
    common::math::Vec3f p1t = p1 - common::math::Vec3f(ray.origin);
    common::math::Vec3f p2t = p2 - common::math::Vec3f(ray.origin);

    // Permute components of triangle vertices and ray direction
    int kz = MaxDim(Abs(ray.dir));
    int kx = (kz + 1) % 3;
    int ky = (kx + 1) % 3;

    common::math::Vec3f d = Permute(ray.dir, kx, ky, kz);
    p0t = Permute(p0t, kx, ky, kz);
    p1t = Permute(p1t, kx, ky, kz);
    p2t = Permute(p2t, kx, ky, kz);

    // Apply shear transformation to translated vertex positions
    Float s_x = -d.x / d.z;
    Float s_y = -d.y / d.z;
    Float s_z = FLOAT_1 / d.z;

    p0t.x += s_x * p0t.z;
    p0t.y += s_y * p0t.z;
    p1t.x += s_x * p1t.z;
    p1t.y += s_y * p1t.z;
    p2t.x += s_x * p2t.z;
    p2t.y += s_y * p2t.z;

    // Compute edge function coefficients _e0_, _e1_, and _e2_
    Float e0 = p1t.x * p2t.y - p1t.y * p2t.x;
    Float e1 = p2t.x * p0t.y - p2t.y * p0t.x;
    Float e2 = p0t.x * p1t.y - p0t.y * p1t.x;

    // Fall back to double precision test at triangle edges
    if (sizeof(Float) == sizeof(float)
        && (FLOAT_0 == e0 || FLOAT_0 == e1 || FLOAT_0 == e2))
    {
        double p2t_x_p1t_y = static_cast<double>(p2t.x) * static_cast<double>(p1t.y);
        double p2t_y_p1t_x = static_cast<double>(p2t.y) * static_cast<double>(p1t.x);
        e0 = (float)(p2t_y_p1t_x - p2t_x_p1t_y);

        double p0t_x_p2t_y = static_cast<double>(p0t.x) * static_cast<double>(p2t.y);
        double p0t_y_p2t_x = static_cast<double>(p0t.y) * static_cast<double>(p2t.x);
        e1 = (float)(p0t_y_p2t_x - p0t_x_p2t_y);

        double p1t_x_p0t_y = static_cast<double>(p1t.x) * static_cast<double>(p0t.y);
        double p1t_y_p0t_x = static_cast<double>(p1t.y) * static_cast<double>(p0t.x);
        e2 = (float)(p1t_y_p0t_x - p1t_x_p0t_y);
    }

    // Perform triangle edge and determinant tests
    if ((e0 < FLOAT_0 || e1 < FLOAT_0 || e2 < FLOAT_0)
        && (e0 > FLOAT_0 || e1 > FLOAT_0 || e2 > FLOAT_0)) return false;

    Float det = e0 + e1 + e2;
    if (FLOAT_0 == det)
    {
        return false;
    }

    // Compute scaled hit distance to triangle and test against ray $t$ range
    p0t.z *= s_z;
    p1t.z *= s_z;
    p2t.z *= s_z;
    Float t_scaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
    if (det < FLOAT_0 && (t_scaled >= ray.t_min || t_scaled < ray.t_max * det)) return false;
    else if (det > FLOAT_0 && (t_scaled <= ray.t_min || t_scaled > ray.t_max * det)) return false;

    // Compute barycentric coordinates and $t$ value for triangle intersection
    Float inv_det = FLOAT_1 / det;
    Float b0 = e0 * inv_det;
    Float b1 = e1 * inv_det;
    Float b2 = e2 * inv_det;
    Float t = t_scaled * inv_det;

    // Ensure that computed triangle $t$ is conservatively greater than zero

    // Compute $\delta_z$ term for triangle $t$ error bounds
    Float max_Zt = MaxComponent(Abs(common::math::Vec3f(p0t.z, p1t.z, p2t.z)));
    Float delta_Z = common::math::Gamma(3) * max_Zt;

    // Compute $\delta_x$ and $\delta_y$ terms for triangle $t$ error bounds
    Float max_Xt = MaxComponent(Abs(common::math::Vec3f(p0t.x, p1t.x, p2t.x)));
    Float delta_X = common::math::Gamma(5) * (max_Xt + max_Zt);

    Float max_Yt = MaxComponent(Abs(common::math::Vec3f(p0t.y, p1t.y, p2t.y)));
    Float delta_Y = common::math::Gamma(5) * (max_Yt + max_Zt);

    // Compute $\delta_e$ term for triangle $t$ error bounds
    Float delta_E = FLOAT_2 * (common::math::Gamma(2) * max_Xt * max_Yt + delta_Y * max_Xt + delta_X * max_Yt);

    // Compute $\delta_t$ term for triangle $t$ error bounds and check _t_
    Float max_E = MaxComponent(Abs(common::math::Vec3f(e0, e1, e2)));
    Float delta_T = FLOAT_3 * (common::math::Gamma(3) * max_E * max_Zt + delta_E * max_Zt
        + delta_Z * max_E) * std::abs(inv_det);
    if (t <= delta_T) return false;

    // Test shadow ray intersection against alpha texture, if present
    if (test_alpha_texture && (nullptr != mesh->alpha_mask || nullptr != mesh->shadow_alpha_mask))
    {
        // Compute triangle partial derivatives
        common::math::Vec3f dp_du, dp_dv;
        common::math::Vec2f uv[3];
        GetUVs(uv);

        // Compute deltas for triangle partial derivatives
        common::math::Vec2f duv02 = uv[0] - uv[2], duv12 = uv[1] - uv[2];
        common::math::Vec3f dp02 = p0 - p2, dp12 = p1 - p2;
        Float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
        bool degenerate_UV = std::abs(determinant) < static_cast<Float>(1e-8);
        if (!degenerate_UV)
        {
            Float inv_det = FLOAT_1 / determinant;
            dp_du = (duv12[1] * dp02 - duv02[1] * dp12) * inv_det;
            dp_dv = (-duv12[0] * dp02 + duv02[0] * dp12) * inv_det;
        }

        if (degenerate_UV || FLOAT_0 == LengthSquared(Cross(dp_du, dp_dv)))
        {
            // Handle zero determinant for triangle partial derivative matrix
            common::math::Vec3f ng = Cross(p2 - p0, p1 - p0);

            // The triangle is actually degenerate; the intersection is bogus.
            if (FLOAT_0 == LengthSquared(ng)) return false;

            CoordinateSystem(Normalize(ng), &dp_du, &dp_dv);
        }

        // Interpolate $(u,v)$ parametric coordinates and hit point
        common::math::Vec3f p_hit = b0 * p0 + b1 * p1 + b2 * p2;
        common::math::Vec2f uv_hit = b0 * uv[0] + b1 * uv[1] + b2 * uv[2];

        interaction::SurfaceInteraction isect_local(p_hit, common::math::Vec3f(FLOAT_0), uv_hit
            , -ray.dir, dp_du, dp_dv, common::math::Vec3f(FLOAT_0), common::math::Vec3f(FLOAT_0)
            , ray.time, this);

        if (nullptr != mesh->alpha_mask
            && FLOAT_0 == mesh->alpha_mask->Evaluate(isect_local))
        {
            return false;
        }

        if (nullptr != mesh->shadow_alpha_mask
            && FLOAT_0 == mesh->shadow_alpha_mask->Evaluate(isect_local))
        {
            return false;
        }
    }
    //++nHits;
    return true;
}


interaction::Interaction Triangle::Sample(const common::math::Vec2f &u, Float *pdf) const
{
    common::math::Vec2f b = sampler::UniformSampleTriangle(u);
    // Get triangle vertices in _p0_, _p1_, and _p2_
    const common::math::Vec3f &p0 = mesh->p[v[0]];
    const common::math::Vec3f &p1 = mesh->p[v[1]];
    const common::math::Vec3f &p2 = mesh->p[v[2]];
    interaction::Interaction it;
    it.p = b[0] * p0 + b[1] * p1 + (1 - b[0] - b[1]) * p2;
    // Compute surface normal for sampled point on triangle
    it.n = Normalize(common::math::Vec3f(Cross(p1 - p0, p2 - p0)));
    // Ensure correct orientation of the geometric normal; follow the same
    // approach as was used in Triangle::Intersect().
    if (mesh->n)
    {
        common::math::Vec3f ns(b[0] * mesh->n[v[0]] + b[1] * mesh->n[v[1]] +
            (FLOAT_1 - b[0] - b[1]) * mesh->n[v[2]]);
        it.n = Faceforward(it.n, ns);
    }
    else if (reverse_orientation ^ transform_swaps_handedness)
    {
        it.n *= -FLOAT_1;
    }

    // Compute error bounds for sampled point on triangle
    common::math::Vec3f pAbsSum = Abs(b[0] * p0) + Abs(b[1] * p1) + Abs((1 - b[0] - b[1]) * p2);
    it.p_error = common::math::Gamma(6) * common::math::Vec3f(pAbsSum.x, pAbsSum.y, pAbsSum.z);
    *pdf = FLOAT_1 / Area();
    return it;
}

Float Triangle::SolidAngle(const common::math::Vec3f &p, int nSamples) const
{
    // Project the vertices into the unit sphere around p.
    std::array<common::math::Vec3f, 3> pSphere = {
        Normalize(mesh->p[v[0]] - p), Normalize(mesh->p[v[1]] - p),
        Normalize(mesh->p[v[2]] - p)
    };

    // http://math.stackexchange.com/questions/9819/area-of-a-spherical-triangle
    // Girard's theorem: surface area of a spherical triangle on a unit
    // sphere is the 'excess angle' alpha+beta+gamma-pi, where
    // alpha/beta/gamma are the interior angles at the vertices.
    //
    // Given three vertices on the sphere, a, b, c, then we can compute,
    // for example, the angle c->a->b by
    //
    // cos theta =  Dot(Cross(c, a), Cross(b, a)) /
    //              (Length(Cross(c, a)) * Length(Cross(b, a))).
    //
    common::math::Vec3f cross01 = (Cross(pSphere[0], pSphere[1]));
    common::math::Vec3f cross12 = (Cross(pSphere[1], pSphere[2]));
    common::math::Vec3f cross20 = (Cross(pSphere[2], pSphere[0]));

    // Some of these vectors may be degenerate. In this case, we don't want
    // to normalize them so that we don't hit an assert. This is fine,
    // since the corresponding dot products below will be zero.
    if (LengthSquared(cross01) > FLOAT_0)
    {
        cross01 = Normalize(cross01);
    }
    if (LengthSquared(cross12) > FLOAT_0)
    {
        cross12 = Normalize(cross12);
    }
    if (LengthSquared(cross20) > FLOAT_0)
    {
        cross20 = Normalize(cross20);
    }

    // We only need to do three cross products to evaluate the angles at
    // all three vertices, though, since we can take advantage of the fact
    // that Cross(a, b) = -Cross(b, a).
    return std::abs(
        std::acos(common::math::Clamp(Dot(cross01, -cross12), -FLOAT_1, FLOAT_1)) +
        std::acos(common::math::Clamp(Dot(cross12, -cross20), -FLOAT_1, FLOAT_1)) +
        std::acos(common::math::Clamp(Dot(cross20, -cross01), -FLOAT_1, FLOAT_1)) - common::math::PI);
}


std::vector<std::shared_ptr<Shape>> CreateTriangleMesh(const common::math::Transformf *object_to_world
    , const common::math::Transformf *world_to_object, bool reverse_orientation, int triangles_number
    , const int *vertice_indices, int vertices_number, const common::math::Vec3f *p
    , const common::math::Vec3f *s, const common::math::Vec3f *n, const common::math::Vec2f *uv
    , const std::shared_ptr<texture::Texture<Float>> &alpha_mask
    , const std::shared_ptr<texture::Texture<Float>> &shadow_alpha_mask, const int *face_indices)
{
    std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>(*object_to_world
        , triangles_number, vertice_indices, vertices_number
        , p, s, n, uv, alpha_mask, shadow_alpha_mask, face_indices);

    std::vector<std::shared_ptr<Shape>> tris;
    tris.reserve(triangles_number);

    for (int i = 0; i < triangles_number; ++i)
    {
        tris.push_back(std::make_shared<Triangle>(object_to_world, world_to_object
            , reverse_orientation, mesh, i));
    }

    return tris;
}
}
}