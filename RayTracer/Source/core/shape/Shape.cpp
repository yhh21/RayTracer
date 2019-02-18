#include "Shape.h"

#include "../../common/math/Transform.h"
#include "../../common/math/Ray.h"
#include "../../common/math/Bounds3.h"
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


}
}