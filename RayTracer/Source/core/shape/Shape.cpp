#include "Shape.h"

#include "../../common/math/Bounds3.h"
#include "../../common/math/Ray.h"
#include "../../common/math/Transform.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Vec3.h"
#include "../../core/interaction/MediumInteraction.h"
#include "../../core/interaction/SurfaceInteraction.h"


namespace core
{
namespace shape
{


Shape::Shape(const common::math::Transformf *object_to_world, const common::math::Transformf *world_to_object
    , bool reverse_orientation)
    : object_to_world(object_to_world), world_to_object(world_to_object)
    , reverse_orientation(reverse_orientation), transform_swaps_handedness(object_to_world->SwapsHandedness())
{
    //++nShapesCreated;
}

common::math::Bounds3f Shape::WorldBound() const
{
    return (*object_to_world)(ObjectBound());
}


interaction::Interaction Shape::Sample(const interaction::Interaction &ref, const common::math::Vec2f &u,
    Float *pdf) const
{
    interaction::Interaction intr = Sample(u, pdf);
    common::math::Vec3f wi = intr.p - ref.p;
    if (FLOAT_0 == LengthSquared(wi))
    {
        *pdf = FLOAT_0;
    }
    else
    {
        wi = Normalize(wi);
        // Convert from area measure, as returned by the Sample() call
        // above, to solid angle measure.
        *pdf *= DistanceSquared(ref.p, intr.p) / AbsDot(intr.n, -wi);
        if (std::isinf(*pdf))
        {
            *pdf = FLOAT_0;
        }
    }

    return intr;
}

Float Shape::Pdf(const interaction::Interaction &ref, const common::math::Vec3f &wi) const
{
    // Intersect sample ray with area light geometry
    common::math::Rayf ray = ref.SpawnRay(wi);
    Float tHit;
    interaction::SurfaceInteraction isectLight;
    // Ignore any alpha textures used for trimming the shape when performing
    // this intersection. Hack for the "San Miguel" scene, where this is used
    // to make an invisible area light.
    if (!Intersect(ray, &tHit, &isectLight, false))
    {
        return FLOAT_0;
    }

    // Convert light sample weight to solid angle measure
    Float pdf = DistanceSquared(ref.p, isectLight.p) /
        (AbsDot(isectLight.n, -wi) * Area());
    if (std::isinf(pdf))
    {
        pdf = FLOAT_0;
    }
    return pdf;
}

Float Shape::SolidAngle(const common::math::Vec3f &p, int nSamples) const
{
    interaction::Interaction ref(p, common::math::Vec3f(), common::math::Vec3f()
        , common::math::Vec3f(FLOAT_0, FLOAT_0, FLOAT_1), FLOAT_0,
        interaction::MediumInterface());
    double solidAngle = 0;
    /* TODO
    for (int i = 0; i < nSamples; ++i)
    {
        common::math::Vec2f u(core::sampler::RadicalInverse(0, i), core::sampler::RadicalInverse(1, i));
        Float pdf;
        interaction::Interaction pShape = Sample(ref, u, &pdf);
        if (pdf > FLOAT_0 && !IntersectP(common::math::Rayf(p, pShape.p - p, static_cast<Float>(0.999F))))
        {
            solidAngle += 1.0f / pdf;
        }
    }
        */
    return solidAngle / nSamples;
}


}
}