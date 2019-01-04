#pragma once

#include "Constants.h"
#include "Mat4.h"
#include "Vec3.h"
#include "Core::Ray.h"

namespace Common
{
    namespace Math
    {
        template<typename T>
        class Transform
        {
        public:
            typedef Mat4<T> Mat4x4T;

            Mat4x4T mat4, inv_mat4;


            ////////////////////////////////////////////////////////////////////////////////
            /// Construction
            ////////////////////////////////////////////////////////////////////////////////

            Transform()
            {}

            Transform(const T m[4][4]) : mat4(Mat4x4T(m)), inv_mat4(Inverse(mat4))
            {}

            Transform(const Mat4x4T &m) : mat4(m), inv_mat4(Inverse(mat4))
            {}

            Transform(const Mat4x4T &m, const Mat4x4T &inv_m) : mat4(m), inv_mat4(inv_m)
            {}


            friend Transform Inverse(const Transform &t)
            {
                return Transform(t.inv_mat4, t.mat4);
            }

            friend Transform Transpose(const Transform &t)
            {
                return Transform(Transpose(t.mat4), Transpose(t.inv_mat4));
            }


            bool SwapsHandedness() const
            {
                Float det = mat4.mat[0][0] * (mat4.mat[1][1] * mat4.mat[2][2] - mat4.mat[1][2] * mat4.mat[2][1])
                    - mat4.mat[0][1] * (mat4.mat[1][0] * mat4.mat[2][2] - mat4.mat[1][2] * mat4.mat[2][0])
                    + mat4.mat[0][2] * (mat4.mat[1][0] * mat4.mat[2][1] - mat4.mat[1][1] * mat4.mat[2][0]);

                return det < 0;
            }
        }


        template <typename T>
        Transform<T> Translate(const Vec3<T> &delta)
        {
            Mat4<T> m(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), delta[0]
                , static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), delta[1]
                , static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), delta[2]
                , static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));

            Mat4<T> inv_m(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), -delta[0]
                , static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), -delta[1]
                , static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), -delta[2]
                , static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));

            return Transform<T>(m, inv_m);
        }

        template <typename T>
        Transform<T> Scale(const Vec3<T> &scale)
        {
            Mat4<T> m(scale[0], static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)
                , static_cast<T>(0), scale[1], static_cast<T>(0), static_cast<T>(0)
                , static_cast<T>(0), static_cast<T>(0), scale[2], static_cast<T>(0)
                , static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));

            Mat4<T> m(static_cast<T>(1) / scale[0], static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)
                , static_cast<T>(0), scale[1], static_cast<T>(0), static_cast<T>(0)
                , static_cast<T>(0), static_cast<T>(0), scale[2], static_cast<T>(0)
                , static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1));

            return Transform<T>(m, inv_m);
        }


        template <typename T>
        Transform<T> Rotate(const T theta, const Vec3<T> &axis)
        {
            Vec3<T> normal_axis = Normalize(axis);

            T sin_theta = std::sin(Radians(theta));
            T cos_theta = std::cos(Radians(theta));

            Mat4<T> mat4;
            /// Compute rotation of first basis vector
            mat4[0][0] = normal_axis[0] * normal_axis[0] + (1 - normal_axis[0] * normal_axis[0]) * cos_theta;
            mat4[0][1] = normal_axis[0] * normal_axis[1] * (1 - cos_theta) - normal_axis[2] * sin_theta;
            mat4[0][2] = normal_axis[0] * normal_axis[2] * (1 - cos_theta) + normal_axis[1] * sin_theta;
            mat4[0][3] = 0;

            /// Compute rotations of second and third basis vectors
            mat4[1][0] = normal_axis[0] * normal_axis[1] * (1 - cos_theta) + normal_axis[2] * sin_theta;
            mat4[1][1] = normal_axis[1] * normal_axis[1] + (1 - normal_axis[1] * normal_axis[1]) * cos_theta;
            mat4[1][2] = normal_axis[1] * normal_axis[2] * (1 - cos_theta) - normal_axis[0] * sin_theta;
            mat4[1][3] = 0;

            mat4[2][0] = normal_axis[0] * normal_axis[2] * (1 - cos_theta) - normal_axis[1] * sin_theta;
            mat4[2][1] = normal_axis[1] * normal_axis[2] * (1 - cos_theta) + normal_axis[0] * sin_theta;
            mat4[2][2] = normal_axis[2] * normal_axis[2] + (1 - normal_axis[2] * normal_axis[2]) * cos_theta;
            mat4[2][3] = 0;

            return Transform<T>(m, Transpose(m));
        }


        template <typename T>
        Transform<T> LookAt(const Vec3<T> &pos, const Vec3<T> &look, const Vec3<T> &up)
        {
            Mat4<T> camera_to_world;
            /// Initialize fourth column of viewing matrix
            camera_to_world[0][3] = pos[0];
            camera_to_world[1][3] = pos[1];
            camera_to_world[2][3] = pos[2];
            camera_to_world[3][3] = 1;

            /// Initialize first three columns of viewing matrix
            Vec3<T> dir = Normalize(look - pos);
            if (Cross(Normalize(up), dir).Length() == 0)
            {
                /*
                Error(
                    "\"up\" vector (%f, %f, %f) and viewing direction (%f, %f, %f) "
                    "passed to LookAt are pointing in the same direction.  Using "
                    "the identity transformation.",
                    up[0], up[1], up[2], dir[0], dir[1], dir[2]);
                    */
                return Transform<T>();
            }

            Vec3<T> right = Normalize(Cross(Normalize(up), dir));
            Vec3<T> new_up = Cross(dir, right);
            camera_to_world[0][0] = right[0];
            camera_to_world[1][0] = right[1];
            camera_to_world[2][0] = right[2];
            camera_to_world[3][0] = static_cast<T>(0);

            camera_to_world[0][1] = new_up[0];
            camera_to_world[1][1] = new_up[1];
            camera_to_world[2][1] = new_up[2];
            camera_to_world[3][1] = static_cast<T>(0);

            camera_to_world[0][2] = dir[0];
            camera_to_world[1][2] = dir[1];
            camera_to_world[2][2] = dir[2];
            camera_to_world[3][2] = static_cast<T>(0);

            return Transform<T>(Inverse(camera_to_world), camera_to_world);
        }


        template <typename T> inline
            Vec3<T> operator()(const Transform<T> &trans, const Vec3<T> &p) const
        {
            T x_p = trans.mat4[0][0] * p[0] + trans.mat4[0][1] * p[1] + trans.mat4[0][2] * p[2] + trans.mat4[0][3];
            T y_p = trans.mat4[1][0] * p[0] + trans.mat4[1][1] * p[1] + trans.mat4[1][2] * p[2] + trans.mat4[1][3];
            T z_p = trans.mat4[2][0] * p[0] + trans.mat4[2][1] * p[1] + trans.mat4[2][2] * p[2] + trans.mat4[2][3];
            T w_p = trans.mat4[3][0] * p[0] + trans.mat4[3][1] * p[1] + trans.mat4[3][2] * p[2] + trans.mat4[3][3];

            if (w_p == static_cast<T>(1)) return Vec3<T>(x_p, y_p, z_p);
            else return Vec3<T>(x_p, y_p, z_p) / w_p;
        }

        template <typename T> inline
            Core::Ray<T> operator()(const Transform<T> &trans, const Core::Ray<T> &ray) const
        {
            Vec3<T> oError;
            Vec3<T> origin = (*this)(ray.origin, &oError);
            Vec3<T> dir = (*this)(ray.dir);
            /// <Offset ray origin to edge of error bounds and compute tMax 233>
            return Core::Ray(origin, dir, tMax, ray.time, ray.medium);
        }

        template <typename T> inline
            Bounds3f operator()(const Transform<T> &trans, const Bounds3f<T> &bounds) const
        {
            Bounds3f ret(trans(Vec3<T>(bounds.point_min.x, bounds.point_min.y, bounds.point_min.z)));
            ret = Union(ret, trans(Vec3<T>(bounds.point_max.x, bounds.point_min.y, bounds.point_min.z)));
            ret = Union(ret, trans(Vec3<T>(bounds.point_min.x, bounds.point_max.y, bounds.point_min.z)));
            ret = Union(ret, trans(Vec3<T>(bounds.point_min.x, bounds.point_min.y, bounds.point_max.z)));
            ret = Union(ret, trans(Vec3<T>(bounds.point_min.x, bounds.point_max.y, bounds.point_max.z)));
            ret = Union(ret, trans(Vec3<T>(bounds.point_max.x, bounds.point_max.y, bounds.point_min.z)));
            ret = Union(ret, trans(Vec3<T>(bounds.point_max.x, bounds.point_min.y, bounds.point_max.z)));
            ret = Union(ret, trans(Vec3<T>(bounds.point_max.x, bounds.point_max.y, bounds.point_max.z)));

            return ret;
        }

        template<typename T> __forceinline
            Transform<T> operator *(const Transform<T> &trans1, const Transform<T> &trans2)
        {
            return Transform<T>(trans1.mat4 * trans2.mat4, trans1.inv_mat4, trans2.inv_mat4);
        }
    }
}