#pragma once

#include "Constants.h"
#include "Mat4.h"
#include "Vec3.h"
#include "Bounds3.h"
#include "Ray.h"
#include "RayDifferential.h"

namespace core
{
    namespace interaction
    {
        class SurfaceInteraction;
    }
}

namespace common
{
namespace math
{


template<typename T>
class Transform
{
public:

    Mat4<T> mat4, inv_mat4;

    ////////////////////////////////////////////////////////////////////////////////
    /// Construction
    ////////////////////////////////////////////////////////////////////////////////

    Transform()
    {}

    Transform(const T m[4][4]) : mat4(Mat4<T>(m)), inv_mat4(Inverse(mat4))
    {}

    Transform(const Mat4<T> &m) : mat4(m), inv_mat4(Inverse(mat4))
    {}

    Transform(const Mat4<T> &m, const Mat4<T> &inv_m) : mat4(m), inv_mat4(inv_m)
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

    inline
        Vec3<T> operator()(const Vec3<T> &v) const
    {
        return Vec3<T>(mat4[0][0] * v.x + mat4[0][1] * v.y + mat4[0][2] * v.z
            , mat4[1][0] * v.x + mat4[1][1] * v.y + mat4[1][2] * v.z
            , mat4[2][0] * v.x + mat4[2][1] * v.y + mat4[2][2] * v.z);
    }

    Ray<T> operator()(const Ray<T> &ray) const
    {
        Vec3<T> origin_error;
        Vec3<T> origin = (*this)(ray.origin, &origin_error);
        Vec3<T> dir = (*this)(ray.dir);

        // Offset ray origin to edge of error bounds and compute _tMax_
        T length_squared = LengthSquared(dir);
        T t_max = ray.t_max;
        if (length_squared > static_cast<T>(0))
        {
            T dt = Dot(Abs(dir), origin_error) / length_squared;
            origin += dir * dt;
            t_max -= dt;
        }

        return Ray(origin, dir, t_max, ray.t_min, ray.time, ray.medium);
    }

    RayDifferential<T> operator()(const RayDifferential<T> &ray_d) const
    {
        RayDifferential<T> ret((*this)(static_cast<Ray<T>>(ray_d)));

        ret.has_differentials = ray_d.has_differentials;
        ret.rx_origin = (*this)(ray_d.rx_origin);
        ret.ry_origin = (*this)(ray_d.ry_origin);
        ret.rx_direction = (*this)(ray_d.rx_direction);
        ret.ry_direction = (*this)(ray_d.ry_direction);

        return ret;
    }

    Bounds3<T> operator()(const Bounds3<T> &bounds) const
    {
        Bounds3<T> ret((*this)(Vec3<T>(bounds.point_min.x, bounds.point_min.y, bounds.point_min.z)));

        ret = Union(ret, (*this)(Vec3<T>(bounds.point_max.x, bounds.point_min.y, bounds.point_min.z)));
        ret = Union(ret, (*this)(Vec3<T>(bounds.point_min.x, bounds.point_max.y, bounds.point_min.z)));
        ret = Union(ret, (*this)(Vec3<T>(bounds.point_min.x, bounds.point_min.y, bounds.point_max.z)));
        ret = Union(ret, (*this)(Vec3<T>(bounds.point_min.x, bounds.point_max.y, bounds.point_max.z)));
        ret = Union(ret, (*this)(Vec3<T>(bounds.point_max.x, bounds.point_max.y, bounds.point_min.z)));
        ret = Union(ret, (*this)(Vec3<T>(bounds.point_max.x, bounds.point_min.y, bounds.point_max.z)));
        ret = Union(ret, (*this)(Vec3<T>(bounds.point_max.x, bounds.point_max.y, bounds.point_max.z)));

        return ret;
    }

    core::interaction::SurfaceInteraction operator()(const core::interaction::SurfaceInteraction &si) const;
};


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



template<typename T> __forceinline
Transform<T> operator *(const Transform<T> &trans1, const Transform<T> &trans2)
{
    return Transform<T>(trans1.mat4 * trans2.mat4, trans1.inv_mat4, trans2.inv_mat4);
}

////////////////////////////////////////////////////////////////////////////////
/// Comparison Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline
bool operator ==(const Transform<T>& a, const Transform<T>& b)
{
    for (int i = 0; i < a.mat4.SIZE; ++i)
    {
        for (int j = 0; j < a.mat4.SIZE; ++j)
        {
            if (a.mat4[i][j] != b.mat4[i][j]) return false;
        }
    }

    return true;
}

template<typename T> __forceinline
bool operator !=(const Transform<T>& a, const Transform<T>& b)
{
    return !(a == b);
}

////////////////////////////////////////////////////////////////////////////////
/// Output Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> inline
std::ostream& operator<<(std::ostream& cout, const Transform<T>& t)
{
    return cout << t.mat4 << endl << t.inv_mat4;
}


}
}