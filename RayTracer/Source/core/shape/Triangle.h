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

            const int nTriangles, nVertices;
            std::vector<int> vertexIndices;
            std::unique_ptr<Vec3f[]> p;
            std::unique_ptr<Vec3f[]> n;
            std::unique_ptr<Vec3f[]> s;
            std::unique_ptr<Vec2f[]> uv;
            std::shared_ptr<Texture<Float>> alphaMask, shadowAlphaMask;
            std::vector<int> faceIndices;


            ////////////////////////////////////////////////////////////////////////////////
            /// Construction
            ////////////////////////////////////////////////////////////////////////////////

            TriangleMesh(const Transform &ObjectToWorld, int nTriangles, const int *vertexIndices
                , int nVertices, const Vec3f *P, const Vec3f *S, const Vec3f *N, const Vec2f *UV
                , const std::shared_ptr<Texture<Float>> &alphaMask, const std::shared_ptr<Texture<Float>> &shadowAlphaMask,
                const int *fIndices)
                : nTriangles(nTriangles), nVertices(nVertices)
                , vertexIndices(vertexIndices, vertexIndices + 3 * nTriangles)
                , alphaMask(alphaMask), shadowAlphaMask(shadowAlphaMask)
            {
                /*
                ++nMeshes;
                nTris += nTriangles;
                triMeshBytes += sizeof(*this) + this->vertexIndices.size() * sizeof(int) +
                    nVertices * (sizeof(*P) + (N ? sizeof(*N) : 0) +
                    (S ? sizeof(*S) : 0) + (UV ? sizeof(*UV) : 0) +
                        (fIndices ? sizeof(*fIndices) : 0));
                */

                /// Transform mesh vertices to world space
                p.reset(new Vec3f[nVertices]);
                for (int i = 0; i < nVertices; ++i)
                {
                    p[i] = ObjectToWorld(P[i]);
                }

                /// Copy _UV_, _N_, and _S_ vertex data, if present
                if (UV)
                {
                    uv.reset(new Vec2f[nVertices]);
                    memcpy(uv.get(), UV, nVertices * sizeof(Vec2f));
                }

                if (N)
                {
                    n.reset(new Vec3f[nVertices]);
                    for (int i = 0; i < nVertices; ++i)
                    {
                        n[i] = ObjectToWorld(N[i]);
                    }
                }

                if (S)
                {
                    s.reset(new Vec3f[nVertices]);
                    for (int i = 0; i < nVertices; ++i)
                    {
                        s[i] = ObjectToWorld(S[i]);
                    }
                }

                if (fIndices)
                {
                    faceIndices = std::vector<int>(fIndices, fIndices + nTriangles);
                }
            }
        };
    
    
        class Triangle : public Shape
        {
        public:

            ////////////////////////////////////////////////////////////////////////////////
            /// Construction
            ////////////////////////////////////////////////////////////////////////////////

            Triangle(const Transform *ObjectToWorld, const Transform *WorldToObject,
                bool reverseOrientation, const std::shared_ptr<TriangleMesh> &mesh,
                int triNumber)
                : Shape(ObjectToWorld, WorldToObject, reverseOrientation), mesh(mesh)
            {
                v = &mesh->vertexIndices[3 * triNumber];
                //triMeshBytes += sizeof(*this);
                faceIndex = mesh->faceIndices.size() ? mesh->faceIndices[triNumber] : 0;
            }

        private:
            std::shared_ptr<TriangleMesh> mesh;
            const int *v;
            int faceIndex;
        };


        std::vector<std::shared_ptr<Shape>> CreateTriangleMesh(
            const Transform *ObjectToWorld, const Transform *WorldToObject,
            bool reverseOrientation, int nTriangles, const int *vertexIndices,
            int nVertices, const Vec3f *p, const Vec3f *s, const Vec3f *n,
            const Vec2f *uv, const std::shared_ptr<Texture<Float>> &alphaMask,
            const std::shared_ptr<Texture<Float>> &shadowAlphaMask,
            const int *faceIndices)
        {
            std::shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>(
                *ObjectToWorld, nTriangles, vertexIndices, nVertices, p, s, n, uv,
                alphaMask, shadowAlphaMask, faceIndices);

            std::vector<std::shared_ptr<Shape>> tris;
            tris.reserve(nTriangles);

            for (int i = 0; i < nTriangles; ++i)
            {
                tris.push_back(std::make_shared<Triangle>(ObjectToWorld, WorldToObject,
                    reverseOrientation, mesh, i));
            }

            return tris;
        }
    }
}