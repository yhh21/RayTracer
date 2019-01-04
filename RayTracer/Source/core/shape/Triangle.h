#pragma once

#include "Shape.h"
#include "common/math/Vec2.h"

class Texture;

namespace Core
{
    namespace Shape
    {
        struct TriangleMesh 
        {

            ////////////////////////////////////////////////////////////////////////////////
            /// TriangleMesh Data
            ////////////////////////////////////////////////////////////////////////////////

            const int triangles_number, vertices_number;
            std::vector<int> vertice_indices;
            std::unique_ptr<Vec3f[]> p;
            std::unique_ptr<Vec3f[]> n;
            std::unique_ptr<Vec3f[]> s;
            std::unique_ptr<Vec2f[]> uv;
            std::shared_ptr<Texture<Float>> alpha_mask, shadow_alpha_mask;
            std::vector<int> face_indices;


            ////////////////////////////////////////////////////////////////////////////////
            /// Construction
            ////////////////////////////////////////////////////////////////////////////////

            TriangleMesh(const Transform &object_to_world, int triangles_number, const int *vertice_indices
                , int vertices_number, const Vec3f *P, const Vec3f *S, const Vec3f *N, const Vec2f *UV
                , const std::shared_ptr<Texture<Float>> &alpha_mask, const std::shared_ptr<Texture<Float>> &shadow_alpha_mask
                , const int *fIndices)
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
                        (fIndices ? sizeof(*fIndices) : 0));
                */

                /// Transform mesh vertices to world space
                p.reset(new Vec3f[vertices_number]);
                for (int i = 0; i < vertices_number; ++i)
                {
                    p[i] = object_to_world(P[i]);
                }

                /// Copy _UV_, _N_, and _S_ vertex data, if present
                if (UV)
                {
                    uv.reset(new Vec2f[vertices_number]);
                    memcpy(uv.get(), UV, vertices_number * sizeof(Vec2f));
                }

                if (N)
                {
                    n.reset(new Vec3f[vertices_number]);
                    for (int i = 0; i < vertices_number; ++i)
                    {
                        n[i] = object_to_world(N[i]);
                    }
                }

                if (S)
                {
                    s.reset(new Vec3f[vertices_number]);
                    for (int i = 0; i < vertices_number; ++i)
                    {
                        s[i] = object_to_world(S[i]);
                    }
                }

                if (fIndices)
                {
                    face_indices = std::vector<int>(fIndices, fIndices + triangles_number);
                }
            }
        };
    
    
        class Triangle : public Shape
        {
        public:

            ////////////////////////////////////////////////////////////////////////////////
            /// Construction
            ////////////////////////////////////////////////////////////////////////////////

            Triangle(const Transform *object_to_world, const Transform *world_to_object,
                bool reverse_orientation, const std::shared_ptr<TriangleMesh> &mesh,
                int triNumber)
                : Shape(object_to_world, world_to_object, reverse_orientation), mesh(mesh)
            {
                v = &mesh->vertice_indices[3 * triNumber];
                //triMeshBytes += sizeof(*this);
                face_index = mesh->face_indices.size() ? mesh->face_indices[triNumber] : 0;
            }

        private:
            std::shared_ptr<TriangleMesh> mesh;
            const int *v;
            int face_index;
        };


        std::vector<std::shared_ptr<Shape>> CreateTriangleMesh(
            const Transform *object_to_world, const Transform *world_to_object,
            bool reverse_orientation, int triangles_number, const int *vertice_indices,
            int vertices_number, const Vec3f *p, const Vec3f *s, const Vec3f *n,
            const Vec2f *uv, const std::shared_ptr<Texture<Float>> &alpha_mask,
            const std::shared_ptr<Texture<Float>> &shadow_alpha_mask,
            const int *face_indices)
        {
            std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>(
                *object_to_world, triangles_number, vertice_indices, vertices_number
                , p, s, n, uv, alpha_mask, shadow_alpha_mask, face_indices);

            std::vector<std::shared_ptr<Shape>> tris;
            tris.reserve(triangles_number);

            for (int i = 0; i < triangles_number; ++i)
            {
                tris.push_back(std::make_shared<Triangle>(object_to_world, world_to_object,
                    reverse_orientation, mesh, i));
            }

            return tris;
        }
    }
}