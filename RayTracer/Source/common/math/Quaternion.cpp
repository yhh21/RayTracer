#include "Quaternion.h"

#include "Mat4.h"

namespace common
{
namespace math
{

template<typename T>
Quaternion<T>::Quaternion(const Transform<T> &t)
{
    const Mat4<T> &mat4 = t.mat4;
    T trace = mat4[0][0] + mat4[1][1] + mat4[2][2];
    if (trace > static_cast<T>(0.0F))
    {
        // Compute w from matrix trace, then xyz
        // 4w^2 = m[0][0] + m[1][1] + m[2][2] + m[3][3] (but m[3][3] == 1)
        T s = std::sqrt(trace + static_cast<T>(1.0F));
        w = s / static_cast<T>(2.0F);
        s = static_cast<T>(0.5F) / s;

        v.x = (mat4[2][1] - mat4[1][2]) * s;
        v.y = (mat4[0][2] - mat4[2][0]) * s;
        v.z = (mat4[1][0] - mat4[0][1]) * s;
    }
    else
    {
        // Compute largest of $x$, $y$, or $z$, then remaining components
        const int nxt[3] = {1, 2, 0};
        T q[3];
        int i = 0;
        if (mat4[1][1] > mat4[0][0])
        {
            i = 1;
        }
        if (mat4[2][2] > mat4[i][i])
        {
            i = 2;
        }
        int j = nxt[i];
        int k = nxt[j];

        T s = std::sqrt((mat4[i][i] - (mat4[j][j] + mat4[k][k])) + static_cast<T>(1.0F));
        q[i] = s * static_cast<T>(0.5F);
        if (static_cast<T>(0.0F) != s)
        {
            s = static_cast<T>(0.5F) / s;
        }

        w = (mat4[k][j] - mat4[j][k]) * s;

        q[j] = (mat4[j][i] + mat4[i][j]) * s;
        q[k] = (mat4[k][i] + mat4[i][k]) * s;

        v.x = q[0];
        v.y = q[1];
        v.z = q[2];
    }
}


template<typename T>
Quaternion<T> Slerp(T t, const Quaternion<T> &q1, const Quaternion<T> &q2)
{
    T cos_theta = Dot(q1, q2);
    if (cos_theta > static_cast<T>(0.9995F))
    {
        return Normalize((1 - t) * q1 + t * q2);
    }
    else
    {
        T theta = std::acos(Clamp(cos_theta, static_cast<T>(-1), static_cast<T>(1)));
        T theta_p = theta * t;
        Quaternion qperp = Normalize(q2 - q1 * cos_theta);

        return q1 * std::cos(theta_p) + qperp * std::sin(theta_p);
    }
}

}
}