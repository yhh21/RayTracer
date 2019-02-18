#pragma once

#include "../../ForwardDeclaration.h"
#include "../../common/math/Vec2.h"
#include "../../common/math/Vec3.h"
#include "../../common/math/Transform.h"
#include "../texture/Texture.h"

namespace core
{
namespace shape
{


struct TriangleMesh
{

    ////////////////////////////////////////////////////////////////////////////////
    // TriangleMesh Data
    ////////////////////////////////////////////////////////////////////////////////

    const int triangles_number, vertices_number;
    std::vector<int> vertice_indices;
    std::unique_ptr<common::math::Vec3f[]> p;
    std::unique_ptr<common::math::Vec3f[]> n;
    std::unique_ptr<common::math::Vec3f[]> s;
    std::unique_ptr<common::math::Vec2f[]> uv;
    std::shared_ptr<texture::Texture<Float>> alpha_mask, shadow_alpha_mask;
    std::vector<int> face_indices;


    ////////////////////////////////////////////////////////////////////////////////
    // Construction
    ////////////////////////////////////////////////////////////////////////////////

    TriangleMesh(const common::math::Transformf &object_to_world, int triangles_number, const int *vertice_indices
        , int vertices_number, const common::math::Vec3f *P, const common::math::Vec3f *S
        , const common::math::Vec3f *N, const common::math::Vec2f *UV
        , const std::shared_ptr<texture::Texture<Float>> &alpha_mask
        , const std::shared_ptr<texture::Texture<Float>> &shadow_alpha_mask
        , const int *f_indices)
        : triangles_number(triangles_number), vertices_number(vertices_number)
        , vertice_indices(vertice_indices, vertice_indices + 3 * triangles_number)
        , alpha_mask(alpha_mask), shadow_alpha_mask(shadow_alpha_mask)
    {
        /*
        ++nMeshes;
        nTris += triangles_number;
        triMeshBytes += sizeof(*this) + this->vertice_indices.size() * sizeof(int) +
            vertices_number * (sizeof(*P) + (N ? sizeof(*N) : 0) +
            (S ? sizeof(*S) : 0) + (UV ? sizeof(*UV) : 0) +
                (f_indices ? sizeof(*f_indices) : 0));
        */

        // common::math::Transformf mesh vertices to world space
        p.reset(new common::math::Vec3f[vertices_number]);
        for (int i = 0; i < vertices_number; ++i)
        {
            p[i] = object_to_world(P[i]);
        }

        // Copy _UV_, _N_, and _S_ vertex data, if present
        if (nullptr != UV)
        {
            uv.reset(new common::math::Vec2f[vertices_number]);
            memcpy(uv.get(), UV, vertices_number * sizeof(common::math::Vec2f));
        }

        if (nullptr != N)
        {
            n.reset(new common::math::Vec3f[vertices_number]);
            for (int i = 0; i < vertices_number; ++i)
            {
                n[i] = object_to_world(N[i]);
            }
        }

        if (nullptr != S)
        {
            s.reset(new common::math::Vec3f[vertices_number]);
            for (int i = 0; i < vertices_number; ++i)
            {
                s[i] = object_to_world(S[i]);
            }
        }

        if (nullptr != f_indices)
        {
            face_indices = std::vector<int>(f_indices, f_indices + triangles_number);
        }
    }
};

}
}