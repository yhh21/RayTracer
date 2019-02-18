#include "Material.h"
#include "../texture/Texture.h"
#include "../interaction/SurfaceInteraction.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Vec3.h"

namespace core
{
namespace material
{



void Material::Bump(const std::shared_ptr<texture::Texture<Float> > &d,
    interaction::SurfaceInteraction *si)
{
    // Compute offset positions and evaluate displacement texture
    interaction::SurfaceInteraction siEval = *si;

    // Shift _siEval_ _du_ in the $u$ direction
    Float du = FLOAT_INV_2 * (std::abs(si->du_dx) + std::abs(si->du_dy));
    // The most common reason for du to be zero is for ray that start from
    // light sources, where no differentials are available. In this case,
    // we try to choose a small enough du so that we still get a decently
    // accurate bump value.
    if (FLOAT_0 == du)
    {
        du = static_cast<Float>(0.0005F);
    }
    siEval.p = si->p + du * si->shading.dp_du;
    siEval.uv = si->uv + common::math::Vec2f(du, FLOAT_0);
    siEval.n = Normalize((common::math::Vec3f)Cross(si->shading.dp_du, si->shading.dp_dv) +
        du * si->dn_du);
    Float uDisplace = d->Evaluate(siEval);

    // Shift _siEval_ _dv_ in the $v$ direction
    Float dv = FLOAT_INV_2 * (std::abs(si->dv_dx) + std::abs(si->dv_dy));
    if (FLOAT_0 == dv)
    {
        dv = static_cast<Float>(0.0005F);
    }
    siEval.p = si->p + dv * si->shading.dp_dv;
    siEval.uv = si->uv + common::math::Vec2f(FLOAT_0, dv);
    siEval.n = Normalize((common::math::Vec3f)Cross(si->shading.dp_du, si->shading.dp_dv)
        + dv * si->dn_dv);
    Float vDisplace = d->Evaluate(siEval);
    Float displace = d->Evaluate(*si);

    // Compute bump-mapped differential geometry
    common::math::Vec3f dp_du = si->shading.dp_du
        + (uDisplace - displace) / du * common::math::Vec3f(si->shading.n)
        + displace * common::math::Vec3f(si->shading.dn_du);

    common::math::Vec3f dp_dv = si->shading.dp_dv +
        (vDisplace - displace) / dv * common::math::Vec3f(si->shading.n)
        + displace * common::math::Vec3f(si->shading.dn_dv);

    si->SetShadingGeometry(dp_du, dp_dv, si->shading.dn_du, si->shading.dn_dv, false);
}


}
}