#pragma once

#include "../../ForwardDeclaration.h"
#include "Shape.h"
#include "TriangleMesh.h"
#include "../../common/math/Vec2.h"

class Texture;

namespace core
{
namespace shape
{


class Triangle : public Shape
{
public:

    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    Triangle(const common::math::Transformf *object_to_world, const common::math::Transformf *world_to_object
        , bool reverse_orientation, const std::shared_ptr<TriangleMesh> &mesh
        , int triNumber)
        : Shape(object_to_world, world_to_object, reverse_orientation), mesh(mesh)
    {
        v = &mesh->vertice_indices[3 * triNumber];
        //triMeshBytes += sizeof(*this);
        face_index = mesh->face_indices.size() ? mesh->face_indices[triNumber] : 0;
    }

    Float Area() const;

    common::math::Bounds3f ObjectBound() const;

    common::math::Bounds3f WorldBound() const;


    bool Intersect(const common::math::Rayf &ray, Float *t_hit, interaction::SurfaceInteraction *isect
        , bool test_alpha_texture) const;

    bool IntersectP(const common::math::Rayf &ray, bool test_alpha_texture) const;

    interaction::Interaction Sample(const common::math::Vec2f &u, Float *pdf) const;

    // Returns the solid angle subtended by the triangle w.r.t. the given
    // reference point p.
    Float SolidAngle(const common::math::Vec3f &p, int nSamples = 0) const;


private:

    void GetUVs(common::math::Vec2f uv[3]) const
    {
        if (nullptr != mesh->uv)
        {
            uv[0] = mesh->uv[v[0]];
            uv[1] = mesh->uv[v[1]];
            uv[2] = mesh->uv[v[2]];
        }
        else
        {
            uv[0] = common::math::Vec2f(FLOAT_0, FLOAT_0);
            uv[1] = common::math::Vec2f(FLOAT_1, FLOAT_0);
            uv[2] = common::math::Vec2f(FLOAT_1, FLOAT_1);
        }
    }

    std::shared_ptr<TriangleMesh> mesh;
    const int *v;
    int face_index;
};


std::vector<std::shared_ptr<Shape>> CreateTriangleMesh(const common::math::Transformf *object_to_world
    , const common::math::Transformf *world_to_object, bool reverse_orientation, int triangles_number
    , const int *vertice_indices, int vertices_number, const common::math::Vec3f *p
    , const common::math::Vec3f *s, const common::math::Vec3f *n, const common::math::Vec2f *uv
    , const std::shared_ptr<texture::Texture<Float>> &alpha_mask
    , const std::shared_ptr<texture::Texture<Float>> &shadow_alpha_mask, const int *face_indices);


}
}