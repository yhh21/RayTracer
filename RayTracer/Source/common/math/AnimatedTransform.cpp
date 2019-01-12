#include "AnimatedTransform.h"

#include "Transform.h"
#include "Quaternion.h"
#include "Mat4.h"
#include "RayDifferential.h"

namespace common
{
namespace math
{


template<typename T>
AnimatedTransform<T>::AnimatedTransform(const Transform<T> *p_start_transform, const T &start_time
    , const Transform<T> *p_end_transform, const T &end_time)
    : p_start_transform(p_start_transform), p_end_transform(p_end_transform)
    , start_time(start_time), end_time(end_time)
    , is_actually_animated(*p_start_transform != *p_end_transform)
{
    if (!is_actually_animated) return;

    translations.resize(2);
    rotations.resize(2);
    scales.resize(2);

    Decompose(p_start_transform->mat4, &(translations[0]), &(rotations[0]), &(scales[0]));
    Decompose(p_end_transform->mat4, &(translations[1]), &(rotations[1]), &(scales[1]));

    /// Flip _R[1]_ if needed to select shortest path
    if (Dot(rotations[0], rotations[1]) < static_cast<T>(0.0F))
    {
        rotations[1] = -rotations[1];
    }
    has_rotation = Dot(rotations[0], rotations[1]) < static_cast<T>(0.9995F);
    // Compute terms of motion derivative function
    if (has_rotation)
    {
        T cos_theta = Dot(rotations[0], rotations[1]);
        T theta = std::acos(Clamp(cos_theta, -1, 1));
        Quaternion<T> qperp = Normalize(rotations[1] - rotations[0] * cos_theta);

        T t0_x = translations[0].x;
        T t0_y = translations[0].y;
        T t0_z = translations[0].z;
        T t1_x = translations[1].x;
        T t1_y = translations[1].y;
        T t1_z = translations[1].z;
        T q1_x = rotations[0].v.x;
        T q1_y = rotations[0].v.y;
        T q1_z = rotations[0].v.z;
        T q1_w = rotations[0].w;
        T qperp_x = qperp.v.x;
        T qperp_y = qperp.v.y;
        T qperp_z = qperp.v.z;
        T qperp_w = qperp.w;
        T s0_00 = (scales[0])[0][0];
        T s0_01 = (scales[0])[0][1];
        T s0_02 = (scales[0])[0][2];
        T s0_10 = (scales[0])[1][0];
        T s0_11 = (scales[0])[1][1];
        T s0_12 = (scales[0])[1][2];
        T s0_20 = (scales[0])[2][0];
        T s0_21 = (scales[0])[2][1];
        T s0_22 = (scales[0])[2][2];
        T s1_00 = (scales[1])[0][0];
        T s1_01 = (scales[1])[0][1];
        T s1_02 = (scales[1])[0][2];
        T s1_10 = (scales[1])[1][0];
        T s1_11 = (scales[1])[1][1];
        T s1_12 = (scales[1])[1][2];
        T s1_20 = (scales[1])[2][0];
        T s1_21 = (scales[1])[2][1];
        T s1_22 = (scales[1])[2][2];

        c1[0] = DerivativeTerm(-t0_x + t1_x
            , (-static_cast<T>(1.0F) + q1_y * q1_y + q1_z * q1_z + qperp_y * qperp_y + qperp_z * qperp_z) * s0_00
            + q1_w * q1_z * s0_10 - qperp_x * qperp_y * s0_10 + qperp_w * qperp_z * s0_10 - q1_w * q1_y * s0_20
            - qperp_w * qperp_y * s0_20 - qperp_x * qperp_z * s0_20 + s1_00 - q1_y * q1_y * s1_00
            - q1_z * q1_z * s1_00 - qperp_y * qperp_y * s1_00 - qperp_z * qperp_z * s1_00 - q1_w * q1_z * s1_10
            + qperp_x * qperp_y * s1_10 - qperp_w * qperp_z * s1_10 + q1_w * q1_y * s1_20 + qperp_w * qperp_y * s1_20
            + qperp_x * qperp_z * s1_20 + q1_x * (-(q1_y * s0_10) - q1_z * s0_20 + q1_y * s1_10 + q1_z * s1_20)
            , (-static_cast<T>(1.0F) + q1_y * q1_y + q1_z * q1_z + qperp_y * qperp_y + qperp_z * qperp_z) * s0_01
            + q1_w * q1_z * s0_11 - qperp_x * qperp_y * s0_11 + qperp_w * qperp_z * s0_11 - q1_w * q1_y * s0_21
            - qperp_w * qperp_y * s0_21 - qperp_x * qperp_z * s0_21 + s1_01 - q1_y * q1_y * s1_01
            - q1_z * q1_z * s1_01 - qperp_y * qperp_y * s1_01 - qperp_z * qperp_z * s1_01 - q1_w * q1_z * s1_11
            + qperp_x * qperp_y * s1_11 - qperp_w * qperp_z * s1_11 + q1_w * q1_y * s1_21 + qperp_w * qperp_y * s1_21
            + qperp_x * qperp_z * s1_21 + q1_x * (-(q1_y * s0_11) - q1_z * s0_21 + q1_y * s1_11 + q1_z * s1_21)
            , (-static_cast<T>(1.0F) + q1_y * q1_y + q1_z * q1_z + qperp_y * qperp_y + qperp_z * qperp_z) * s0_02
            + q1_w * q1_z * s0_12 - qperp_x * qperp_y * s0_12 + qperp_w * qperp_z * s0_12 - q1_w * q1_y * s0_22
            - qperp_w * qperp_y * s0_22 - qperp_x * qperp_z * s0_22 + s1_02 - q1_y * q1_y * s1_02
            - q1_z * q1_z * s1_02 - qperp_y * qperp_y * s1_02 - qperp_z * qperp_z * s1_02 - q1_w * q1_z * s1_12
            + qperp_x * qperp_y * s1_12 - qperp_w * qperp_z * s1_12 + q1_w * q1_y * s1_22 + qperp_w * qperp_y * s1_22
            + qperp_x * qperp_z * s1_22 + q1_x * (-(q1_y * s0_12) - q1_z * s0_22 + q1_y * s1_12 + q1_z * s1_22));

        c2[0] = DerivativeTerm(static_cast<T>(0.0F), -(qperp_y * qperp_y * s0_00) - qperp_z * qperp_z * s0_00
            + qperp_x * qperp_y * s0_10 - qperp_w * qperp_z * s0_10 + qperp_w * qperp_y * s0_20
            + qperp_x * qperp_z * s0_20 + q1_y * q1_y * (s0_00 - s1_00) + q1_z * q1_z * (s0_00 - s1_00)
            + qperp_y * qperp_y * s1_00 + qperp_z * qperp_z * s1_00 - qperp_x * qperp_y * s1_10
            + qperp_w * qperp_z * s1_10 - qperp_w * qperp_y * s1_20 - qperp_x * qperp_z * s1_20
            + static_cast<T>(2.0F) * q1_x * qperp_y * s0_10 * theta - static_cast<T>(2.0F) * q1_w * qperp_z * s0_10 * theta
            + static_cast<T>(2.0F) * q1_w * qperp_y * s0_20 * theta + static_cast<T>(2.0F) * q1_x * qperp_z * s0_20 * theta
            + q1_y * (q1_x * (-s0_10 + s1_10) + q1_w * (-s0_20 + s1_20)
                + static_cast<T>(2.0F) * (-static_cast<T>(2.0F) * qperp_y * s0_00
                + qperp_x * s0_10 + qperp_w * s0_20) * theta) + q1_z * (q1_w * (s0_10 - s1_10)
                    + q1_x * (-s0_20 + s1_20) - static_cast<T>(2.0F) * (static_cast<T>(2.0F) * qperp_z * s0_00
                + qperp_w * s0_10 - qperp_x * s0_20) * theta)
            , -(qperp_y * qperp_y * s0_01) - qperp_z * qperp_z * s0_01 + qperp_x * qperp_y * s0_11
            - qperp_w * qperp_z * s0_11 + qperp_w * qperp_y * s0_21 + qperp_x * qperp_z * s0_21
            + q1_y * q1_y * (s0_01 - s1_01) + q1_z * q1_z * (s0_01 - s1_01) + qperp_y * qperp_y * s1_01
            + qperp_z * qperp_z * s1_01 - qperp_x * qperp_y * s1_11 + qperp_w * qperp_z * s1_11
            - qperp_w * qperp_y * s1_21 - qperp_x * qperp_z * s1_21 + static_cast<T>(2.0F) * q1_x * qperp_y * s0_11 * theta
            - static_cast<T>(2.0F) * q1_w * qperp_z * s0_11 * theta + static_cast<T>(2.0F) * q1_w * qperp_y * s0_21 * theta
            + static_cast<T>(2.0F) * q1_x * qperp_z * s0_21 * theta + q1_y * (q1_x * (-s0_11 + s1_11) + q1_w * (-s0_21 + s1_21)
                + static_cast<T>(2.0F) * (-static_cast<T>(2.0F) * qperp_y * s0_01 + qperp_x * s0_11 + qperp_w * s0_21) * theta)
            + q1_z * (q1_w * (s0_11 - s1_11) + q1_x * (-s0_21 + s1_21)
                - static_cast<T>(2.0F) * (static_cast<T>(2.0F) * qperp_z * s0_01
                + qperp_w * s0_11 - qperp_x * s0_21) * theta)
            , -(qperp_y * qperp_y * s0_02) - qperp_z * qperp_z * s0_02 + qperp_x * qperp_y * s0_12
            - qperp_w * qperp_z * s0_12 + qperp_w * qperp_y * s0_22 + qperp_x * qperp_z * s0_22
            + q1_y * q1_y * (s0_02 - s1_02) + q1_z * q1_z * (s0_02 - s1_02) + qperp_y * qperp_y * s1_02
            + qperp_z * qperp_z * s1_02 - qperp_x * qperp_y * s1_12 + qperp_w * qperp_z * s1_12 -
            qperp_w * qperp_y * s1_22 - qperp_x * qperp_z * s1_22 + static_cast<T>(2.0F) * q1_x * qperp_y * s0_12 * theta
            - static_cast<T>(2.0F) * q1_w * qperp_z * s0_12 * theta + static_cast<T>(2.0F) * q1_w * qperp_y * s0_22 * theta
            + static_cast<T>(2.0F) * q1_x * qperp_z * s0_22 * theta + q1_y * (q1_x * (-s0_12 + s1_12) + q1_w * (-s0_22 + s1_22)
                + static_cast<T>(2.0F) * (-static_cast<T>(2.0F) * qperp_y * s0_02 + qperp_x * s0_12 + qperp_w * s0_22) * theta)
            + q1_z * (q1_w * (s0_12 - s1_12) + q1_x * (-s0_22 + s1_22)
                - static_cast<T>(2.0F) * (static_cast<T>(2.0F) * qperp_z * s0_02 + qperp_w * s0_12 - qperp_x * s0_22) * theta));

        c3[0] = DerivativeTerm(static_cast<T>(0.0F), -static_cast<T>(2.0F) * (q1_x * qperp_y * s0_10 - q1_w * qperp_z * s0_10
            + q1_w * qperp_y * s0_20 + q1_x * qperp_z * s0_20 - q1_x * qperp_y * s1_10 + q1_w * qperp_z * s1_10
            - q1_w * qperp_y * s1_20 - q1_x * qperp_z * s1_20 + q1_y * (-static_cast<T>(2.0F) * qperp_y * s0_00
                + qperp_x * s0_10 + qperp_w * s0_20 + static_cast<T>(2.0F) * qperp_y * s1_00 - qperp_x * s1_10 - qperp_w * s1_20)
            + q1_z * (-static_cast<T>(2.0F) * qperp_z * s0_00 - qperp_w * s0_10 + qperp_x * s0_20
                + static_cast<T>(2.0F) * qperp_z * s1_00 + qperp_w * s1_10 - qperp_x * s1_20)) * theta
            , -static_cast<T>(2.0F) * (q1_x * qperp_y * s0_11 - q1_w * qperp_z * s0_11 + q1_w * qperp_y * s0_21
                + q1_x * qperp_z * s0_21 - q1_x * qperp_y * s1_11 + q1_w * qperp_z * s1_11 - q1_w * qperp_y * s1_21
                - q1_x * qperp_z * s1_21 + q1_y * (-static_cast<T>(2.0F) * qperp_y * s0_01 + qperp_x * s0_11 + qperp_w * s0_21
                    + static_cast<T>(2.0F) * qperp_y * s1_01 - qperp_x * s1_11 - qperp_w * s1_21)
                + q1_z * (-static_cast<T>(2.0F) * qperp_z * s0_01 - qperp_w * s0_11 + qperp_x * s0_21
                    + static_cast<T>(2.0F) * qperp_z * s1_01 + qperp_w * s1_11 - qperp_x * s1_21)) * theta
            , -static_cast<T>(2.0F) * (q1_x * qperp_y * s0_12 - q1_w * qperp_z * s0_12 + q1_w * qperp_y * s0_22
                + q1_x * qperp_z * s0_22 - q1_x * qperp_y * s1_12 + q1_w * qperp_z * s1_12 - q1_w * qperp_y * s1_22
                - q1_x * qperp_z * s1_22 + q1_y * (-static_cast<T>(2.0F) * qperp_y * s0_02 + qperp_x * s0_12 + qperp_w * s0_22
                    + static_cast<T>(2.0F) * qperp_y * s1_02 - qperp_x * s1_12 - qperp_w * s1_22)
                + q1_z * (-static_cast<T>(2.0F) * qperp_z * s0_02 - qperp_w * s0_12 + qperp_x * s0_22
                    + static_cast<T>(2.0F) * qperp_z * s1_02 + qperp_w * s1_12 - qperp_x * s1_22)) * theta);

        c4[0] = DerivativeTerm(static_cast<T>(0.0F), -(q1_x * qperp_y * s0_10) + q1_w * qperp_z * s0_10 - q1_w * qperp_y * s0_20
            - q1_x * qperp_z * s0_20 + q1_x * qperp_y * s1_10 - q1_w * qperp_z * s1_10 + q1_w * qperp_y * s1_20
            + q1_x * qperp_z * s1_20 + static_cast<T>(2.0F) * q1_y * q1_y * s0_00 * theta
            + static_cast<T>(2.0F) * q1_z * q1_z * s0_00 * theta
            - static_cast<T>(2.0F) * qperp_y * qperp_y * s0_00 * theta - static_cast<T>(2.0F) * qperp_z * qperp_z * s0_00 * theta
            + static_cast<T>(2.0F) * qperp_x * qperp_y * s0_10 * theta - static_cast<T>(2.0F) * qperp_w * qperp_z * s0_10 * theta
            + static_cast<T>(2.0F) * qperp_w * qperp_y * s0_20 * theta + static_cast<T>(2.0F) * qperp_x * qperp_z * s0_20 * theta
            + q1_y * (-(qperp_x * s0_10) - qperp_w * s0_20 + static_cast<T>(2.0F) * qperp_y * (s0_00 - s1_00) + qperp_x * s1_10
                + qperp_w * s1_20 - static_cast<T>(2.0F) * q1_x * s0_10 * theta - static_cast<T>(2.0F) * q1_w * s0_20 * theta)
            + q1_z * (static_cast<T>(2.0F) * qperp_z * s0_00 + qperp_w * s0_10 - qperp_x * s0_20
                - static_cast<T>(2.0F) * qperp_z * s1_00
                - qperp_w * s1_10 + qperp_x * s1_20 + static_cast<T>(2.0F) * q1_w * s0_10 * theta
                - static_cast<T>(2.0F) * q1_x * s0_20 * theta)
            , -(q1_x * qperp_y * s0_11) + q1_w * qperp_z * s0_11 - q1_w * qperp_y * s0_21 - q1_x * qperp_z * s0_21
            + q1_x * qperp_y * s1_11 - q1_w * qperp_z * s1_11 + q1_w * qperp_y * s1_21 + q1_x * qperp_z * s1_21
            + static_cast<T>(2.0F) * q1_y * q1_y * s0_01 * theta + static_cast<T>(2.0F) * q1_z * q1_z * s0_01 * theta
            - static_cast<T>(2.0F) * qperp_y * qperp_y * s0_01 * theta - static_cast<T>(2.0F) * qperp_z * qperp_z * s0_01 * theta
            + static_cast<T>(2.0F) * qperp_x * qperp_y * s0_11 * theta - static_cast<T>(2.0F) * qperp_w * qperp_z * s0_11 * theta
            + static_cast<T>(2.0F) * qperp_w * qperp_y * s0_21 * theta + static_cast<T>(2.0F) * qperp_x * qperp_z * s0_21 * theta
            + q1_y * (-(qperp_x * s0_11) - qperp_w * s0_21 + static_cast<T>(2.0F) * qperp_y * (s0_01 - s1_01) + qperp_x * s1_11
                + qperp_w * s1_21 - static_cast<T>(2.0F) * q1_x * s0_11 * theta - static_cast<T>(2.0F) * q1_w * s0_21 * theta)
            + q1_z * (static_cast<T>(2.0F) * qperp_z * s0_01 + qperp_w * s0_11 - qperp_x * s0_21
                - static_cast<T>(2.0F) * qperp_z * s1_01
                - qperp_w * s1_11 + qperp_x * s1_21 + static_cast<T>(2.0F) * q1_w * s0_11 * theta
                - static_cast<T>(2.0F) * q1_x * s0_21 * theta)
            , -(q1_x * qperp_y * s0_12) + q1_w * qperp_z * s0_12 - q1_w * qperp_y * s0_22 - q1_x * qperp_z * s0_22
            + q1_x * qperp_y * s1_12 - q1_w * qperp_z * s1_12 + q1_w * qperp_y * s1_22 + q1_x * qperp_z * s1_22
            + static_cast<T>(2.0F) * q1_y * q1_y * s0_02 * theta + static_cast<T>(2.0F) * q1_z * q1_z * s0_02 * theta
            - static_cast<T>(2.0F) * qperp_y * qperp_y * s0_02 * theta - static_cast<T>(2.0F) * qperp_z * qperp_z * s0_02 * theta
            + static_cast<T>(2.0F) * qperp_x * qperp_y * s0_12 * theta - static_cast<T>(2.0F) * qperp_w * qperp_z * s0_12 * theta
            + static_cast<T>(2.0F) * qperp_w * qperp_y * s0_22 * theta + static_cast<T>(2.0F) * qperp_x * qperp_z * s0_22 * theta
            + q1_y * (-(qperp_x * s0_12) - qperp_w * s0_22 + static_cast<T>(2.0F) * qperp_y * (s0_02 - s1_02) + qperp_x * s1_12
                + qperp_w * s1_22 - static_cast<T>(2.0F) * q1_x * s0_12 * theta - static_cast<T>(2.0F) * q1_w * s0_22 * theta)
            + q1_z * (static_cast<T>(2.0F) * qperp_z * s0_02 + qperp_w * s0_12 - qperp_x * s0_22
                - static_cast<T>(2.0F) * qperp_z * s1_02 - qperp_w * s1_12 + qperp_x * s1_22
                + static_cast<T>(2.0F) * q1_w * s0_12 * theta - static_cast<T>(2.0F) * q1_x * s0_22 * theta));

        c5[0] = DerivativeTerm(static_cast<T>(0.0F), static_cast<T>(2.0F) * (qperp_y * qperp_y * s0_00 + qperp_z * qperp_z * s0_00
            - qperp_x * qperp_y * s0_10 + qperp_w * qperp_z * s0_10 - qperp_w * qperp_y * s0_20 - qperp_x * qperp_z * s0_20
            - qperp_y * qperp_y * s1_00 - qperp_z * qperp_z * s1_00 + q1_y * q1_y * (-s0_00 + s1_00) + q1_z * q1_z * (-s0_00 + s1_00)
            + qperp_x * qperp_y * s1_10 - qperp_w * qperp_z * s1_10 + q1_y * (q1_x * (s0_10 - s1_10) + q1_w * (s0_20 - s1_20))
            + qperp_w * qperp_y * s1_20 + qperp_x * qperp_z * s1_20 + q1_z * (-(q1_w * s0_10) + q1_x * s0_20 + q1_w * s1_10
                - q1_x * s1_20)) * theta
            , static_cast<T>(2.0F) * (qperp_y * qperp_y * s0_01 + qperp_z * qperp_z * s0_01 - qperp_x * qperp_y * s0_11
                + qperp_w * qperp_z * s0_11 - qperp_w * qperp_y * s0_21 - qperp_x * qperp_z * s0_21 - qperp_y * qperp_y * s1_01
                - qperp_z * qperp_z * s1_01 + q1_y * q1_y * (-s0_01 + s1_01) + q1_z * q1_z * (-s0_01 + s1_01)
                + qperp_x * qperp_y * s1_11 - qperp_w * qperp_z * s1_11 + q1_y * (q1_x * (s0_11 - s1_11) + q1_w * (s0_21 - s1_21))
                + qperp_w * qperp_y * s1_21 + qperp_x * qperp_z * s1_21 + q1_z * (-(q1_w * s0_11) + q1_x * s0_21
                    + q1_w * s1_11 - q1_x * s1_21)) * theta
            , static_cast<T>(2.0F) * (qperp_y * qperp_y * s0_02 + qperp_z * qperp_z * s0_02 - qperp_x * qperp_y * s0_12
                + qperp_w * qperp_z * s0_12 - qperp_w * qperp_y * s0_22 - qperp_x * qperp_z * s0_22 - qperp_y * qperp_y * s1_02
                - qperp_z * qperp_z * s1_02 + q1_y * q1_y * (-s0_02 + s1_02) + q1_z * q1_z * (-s0_02 + s1_02)
                + qperp_x * qperp_y * s1_12 - qperp_w * qperp_z * s1_12 + q1_y * (q1_x * (s0_12 - s1_12) + q1_w * (s0_22 - s1_22))
                + qperp_w * qperp_y * s1_22 + qperp_x * qperp_z * s1_22 + q1_z * (-(q1_w * s0_12) + q1_x * s0_22
                    + q1_w * s1_12 - q1_x * s1_22)) * theta);

        c1[1] = DerivativeTerm(-t0_y + t1_y, -(qperp_x * qperp_y * s0_00) - qperp_w * qperp_z * s0_00 - s0_10
            + q1_z * q1_z * s0_10 + qperp_x * qperp_x * s0_10 + qperp_z * qperp_z * s0_10 - q1_y * q1_z * s0_20
            + qperp_w * qperp_x * s0_20 - qperp_y * qperp_z * s0_20 + qperp_x * qperp_y * s1_00 + qperp_w * qperp_z * s1_00
            + q1_w * q1_z * (-s0_00 + s1_00) + q1_x * q1_x * (s0_10 - s1_10) + s1_10 - q1_z * q1_z * s1_10
            - qperp_x * qperp_x * s1_10 - qperp_z * qperp_z * s1_10 + q1_x * (q1_y * (-s0_00 + s1_00) + q1_w * (s0_20 - s1_20))
            + q1_y * q1_z * s1_20 - qperp_w * qperp_x * s1_20 + qperp_y * qperp_z * s1_20
            , -(qperp_x * qperp_y * s0_01) - qperp_w * qperp_z * s0_01 - s0_11 + q1_z * q1_z * s0_11 + qperp_x * qperp_x * s0_11
            + qperp_z * qperp_z * s0_11 - q1_y * q1_z * s0_21 + qperp_w * qperp_x * s0_21 - qperp_y * qperp_z * s0_21
            + qperp_x * qperp_y * s1_01 + qperp_w * qperp_z * s1_01 + q1_w * q1_z * (-s0_01 + s1_01) + q1_x * q1_x * (s0_11 - s1_11)
            + s1_11 - q1_z * q1_z * s1_11 - qperp_x * qperp_x * s1_11 - qperp_z * qperp_z * s1_11 + q1_x * (q1_y * (-s0_01 + s1_01)
                + q1_w * (s0_21 - s1_21)) + q1_y * q1_z * s1_21 - qperp_w * qperp_x * s1_21 + qperp_y * qperp_z * s1_21
            , -(qperp_x * qperp_y * s0_02) - qperp_w * qperp_z * s0_02 - s0_12 + q1_z * q1_z * s0_12 + qperp_x * qperp_x * s0_12
            + qperp_z * qperp_z * s0_12 - q1_y * q1_z * s0_22 + qperp_w * qperp_x * s0_22 - qperp_y * qperp_z * s0_22
            + qperp_x * qperp_y * s1_02 + qperp_w * qperp_z * s1_02 + q1_w * q1_z * (-s0_02 + s1_02) + q1_x * q1_x * (s0_12 - s1_12)
            + s1_12 - q1_z * q1_z * s1_12 - qperp_x * qperp_x * s1_12 - qperp_z * qperp_z * s1_12 + q1_x * (q1_y * (-s0_02 + s1_02)
                + q1_w * (s0_22 - s1_22)) + q1_y * q1_z * s1_22 - qperp_w * qperp_x * s1_22 + qperp_y * qperp_z * s1_22);

        c2[1] = DerivativeTerm(static_cast<T>(0.0F), qperp_x * qperp_y * s0_00 + qperp_w * qperp_z * s0_00 + q1_z * q1_z * s0_10
            - qperp_x * qperp_x * s0_10 - qperp_z * qperp_z * s0_10 - q1_y * q1_z * s0_20 - qperp_w * qperp_x * s0_20
            + qperp_y * qperp_z * s0_20 - qperp_x * qperp_y * s1_00 - qperp_w * qperp_z * s1_00 + q1_x * q1_x * (s0_10 - s1_10)
            - q1_z * q1_z * s1_10 + qperp_x * qperp_x * s1_10 + qperp_z * qperp_z * s1_10 + q1_y * q1_z * s1_20
            + qperp_w * qperp_x * s1_20 - qperp_y * qperp_z * s1_20 + static_cast<T>(2.0F) * q1_z * qperp_w * s0_00 * theta
            + static_cast<T>(2.0F) * q1_y * qperp_x * s0_00 * theta - static_cast<T>(4.0F) * q1_z * qperp_z * s0_10 * theta
            + static_cast<T>(2.0F) * q1_z * qperp_y * s0_20 * theta + static_cast<T>(2.0F) * q1_y * qperp_z * s0_20 * theta
            + q1_x * (q1_w * s0_20 + q1_y * (-s0_00 + s1_00) - q1_w * s1_20 + static_cast<T>(2.0F) * qperp_y * s0_00 * theta
                - static_cast<T>(4.0F) * qperp_x * s0_10 * theta - static_cast<T>(2.0F) * qperp_w * s0_20 * theta)
            + q1_w * (-(q1_z * s0_00) + q1_z * s1_00 + static_cast<T>(2.0F) * qperp_z * s0_00 * theta
                - static_cast<T>(2.0F) * qperp_x * s0_20 * theta)
            , qperp_x * qperp_y * s0_01 + qperp_w * qperp_z * s0_01 + q1_z * q1_z * s0_11 - qperp_x * qperp_x * s0_11
            - qperp_z * qperp_z * s0_11 - q1_y * q1_z * s0_21 - qperp_w * qperp_x * s0_21 + qperp_y * qperp_z * s0_21
            - qperp_x * qperp_y * s1_01 - qperp_w * qperp_z * s1_01 + q1_x * q1_x * (s0_11 - s1_11) - q1_z * q1_z * s1_11
            + qperp_x * qperp_x * s1_11 + qperp_z * qperp_z * s1_11 + q1_y * q1_z * s1_21 + qperp_w * qperp_x * s1_21
            - qperp_y * qperp_z * s1_21 + static_cast<T>(2.0F) * q1_z * qperp_w * s0_01 * theta
            + static_cast<T>(2.0F) * q1_y * qperp_x * s0_01 * theta
            - static_cast<T>(4.0F) * q1_z * qperp_z * s0_11 * theta + static_cast<T>(2.0F) * q1_z * qperp_y * s0_21 * theta
            + static_cast<T>(2.0F) * q1_y * qperp_z * s0_21 * theta + q1_x * (q1_w * s0_21 + q1_y * (-s0_01 + s1_01) - q1_w * s1_21
                + static_cast<T>(2.0F) * qperp_y * s0_01 * theta - static_cast<T>(4.0F) * qperp_x * s0_11 * theta
                - static_cast<T>(2.0F) * qperp_w * s0_21 * theta) + q1_w * (-(q1_z * s0_01) + q1_z * s1_01
                    + static_cast<T>(2.0F) * qperp_z * s0_01 * theta
                    - static_cast<T>(2.0F) * qperp_x * s0_21 * theta)
            , qperp_x * qperp_y * s0_02 + qperp_w * qperp_z * s0_02 + q1_z * q1_z * s0_12 - qperp_x * qperp_x * s0_12
            - qperp_z * qperp_z * s0_12 - q1_y * q1_z * s0_22 - qperp_w * qperp_x * s0_22 + qperp_y * qperp_z * s0_22
            - qperp_x * qperp_y * s1_02 - qperp_w * qperp_z * s1_02 + q1_x * q1_x * (s0_12 - s1_12) - q1_z * q1_z * s1_12
            + qperp_x * qperp_x * s1_12 + qperp_z * qperp_z * s1_12 + q1_y * q1_z * s1_22 + qperp_w * qperp_x * s1_22
            - qperp_y * qperp_z * s1_22 + static_cast<T>(2.0F) * q1_z * qperp_w * s0_02 * theta
            + static_cast<T>(2.0F) * q1_y * qperp_x * s0_02 * theta
            - static_cast<T>(4.0F) * q1_z * qperp_z * s0_12 * theta + static_cast<T>(2.0F) * q1_z * qperp_y * s0_22 * theta
            + static_cast<T>(2.0F) * q1_y * qperp_z * s0_22 * theta + q1_x * (q1_w * s0_22 + q1_y * (-s0_02 + s1_02) - q1_w * s1_22
                + static_cast<T>(2.0F) * qperp_y * s0_02 * theta - static_cast<T>(4.0F) * qperp_x * s0_12 * theta
                - static_cast<T>(2.0F) * qperp_w * s0_22 * theta) + q1_w * (-(q1_z * s0_02) + q1_z * s1_02
                    + static_cast<T>(2.0F) * qperp_z * s0_02 * theta
                    - static_cast<T>(2.0F) * qperp_x * s0_22 * theta));

        c3[1] = DerivativeTerm(static_cast<T>(0.0F), static_cast<T>(2.0F) * (-(q1_x * qperp_y * s0_00) - q1_w * qperp_z * s0_00
            + static_cast<T>(2.0F) * q1_x * qperp_x * s0_10 + q1_x * qperp_w * s0_20 + q1_w * qperp_x * s0_20
            + q1_x * qperp_y * s1_00 + q1_w * qperp_z * s1_00 - static_cast<T>(2.0F) * q1_x * qperp_x * s1_10
            - q1_x * qperp_w * s1_20 - q1_w * qperp_x * s1_20 + q1_z * (static_cast<T>(2.0F) * qperp_z * s0_10
                - qperp_y * s0_20 + qperp_w * (-s0_00 + s1_00) - static_cast<T>(2.0F) * qperp_z * s1_10
                + qperp_y * s1_20) + q1_y * (-(qperp_x * s0_00) - qperp_z * s0_20 + qperp_x * s1_00 + qperp_z * s1_20)) * theta
            , static_cast<T>(2.0F) * (-(q1_x * qperp_y * s0_01) - q1_w * qperp_z * s0_01
                + static_cast<T>(2.0F) * q1_x * qperp_x * s0_11 + q1_x * qperp_w * s0_21
                + q1_w * qperp_x * s0_21 + q1_x * qperp_y * s1_01 + q1_w * qperp_z * s1_01
                - static_cast<T>(2.0F) * q1_x * qperp_x * s1_11 - q1_x * qperp_w * s1_21 - q1_w * qperp_x * s1_21
                + q1_z * (static_cast<T>(2.0F) * qperp_z * s0_11 - qperp_y * s0_21
                    + qperp_w * (-s0_01 + s1_01) - static_cast<T>(2.0F) * qperp_z * s1_11 + qperp_y * s1_21)
                + q1_y * (-(qperp_x * s0_01) - qperp_z * s0_21 + qperp_x * s1_01 + qperp_z * s1_21)) * theta
            , static_cast<T>(2.0F) * (-(q1_x * qperp_y * s0_02) - q1_w * qperp_z * s0_02 
                + static_cast<T>(2.0F) * q1_x * qperp_x * s0_12 + q1_x * qperp_w * s0_22
                + q1_w * qperp_x * s0_22 + q1_x * qperp_y * s1_02 + q1_w * qperp_z * s1_02
                - static_cast<T>(2.0F) * q1_x * qperp_x * s1_12 - q1_x * qperp_w * s1_22 - q1_w * qperp_x * s1_22
                + q1_z * (static_cast<T>(2.0F) * qperp_z * s0_12 - qperp_y * s0_22
                    + qperp_w * (-s0_02 + s1_02) - static_cast<T>(2.0F) * qperp_z * s1_12 + qperp_y * s1_22)
                + q1_y * (-(qperp_x * s0_02) - qperp_z * s0_22 + qperp_x * s1_02 + qperp_z * s1_22)) * theta);

        c4[1] = DerivativeTerm(static_cast<T>(0.0F), -(q1_x * qperp_y * s0_00) - q1_w * qperp_z * s0_00
            + static_cast<T>(2.0F) * q1_x * qperp_x * s0_10 + q1_x * qperp_w * s0_20 + q1_w * qperp_x * s0_20
            + q1_x * qperp_y * s1_00 + q1_w * qperp_z * s1_00 - static_cast<T>(2.0F) * q1_x * qperp_x * s1_10
            - q1_x * qperp_w * s1_20 - q1_w * qperp_x * s1_20 + static_cast<T>(2.0F) * qperp_x * qperp_y * s0_00 * theta
            + static_cast<T>(2.0F) * qperp_w * qperp_z * s0_00 * theta + static_cast<T>(2.0F) * q1_x * q1_x * s0_10 * theta
            + static_cast<T>(2.0F) * q1_z * q1_z * s0_10 * theta - static_cast<T>(2.0F) * qperp_x * qperp_x * s0_10 * theta
            - static_cast<T>(2.0F) * qperp_z * qperp_z * s0_10 * theta + static_cast<T>(2.0F) * q1_w * q1_x * s0_20 * theta
            - static_cast<T>(2.0F) * qperp_w * qperp_x * s0_20 * theta + static_cast<T>(2.0F) * qperp_y * qperp_z * s0_20 * theta
            + q1_y * (-(qperp_x * s0_00) - qperp_z * s0_20 + qperp_x * s1_00 + qperp_z * s1_20
                - static_cast<T>(2.0F) * q1_x * s0_00 * theta) + q1_z * (static_cast<T>(2.0F) * qperp_z * s0_10
                    - qperp_y * s0_20 + qperp_w * (-s0_00 + s1_00) - static_cast<T>(2.0F) * qperp_z * s1_10 + qperp_y * s1_20
                    - static_cast<T>(2.0F) * q1_w * s0_00 * theta - static_cast<T>(2.0F) * q1_y * s0_20 * theta)
            , -(q1_x * qperp_y * s0_01) - q1_w * qperp_z * s0_01 + static_cast<T>(2.0F) * q1_x * qperp_x * s0_11
            + q1_x * qperp_w * s0_21 + q1_w * qperp_x * s0_21 + q1_x * qperp_y * s1_01 + q1_w * qperp_z * s1_01
            - static_cast<T>(2.0F) * q1_x * qperp_x * s1_11 - q1_x * qperp_w * s1_21 - q1_w * qperp_x * s1_21
            + static_cast<T>(2.0F) * qperp_x * qperp_y * s0_01 * theta + static_cast<T>(2.0F) * qperp_w * qperp_z * s0_01 * theta
            + static_cast<T>(2.0F) * q1_x * q1_x * s0_11 * theta + static_cast<T>(2.0F) * q1_z * q1_z * s0_11 * theta
            - static_cast<T>(2.0F) * qperp_x * qperp_x * s0_11 * theta - static_cast<T>(2.0F) * qperp_z * qperp_z * s0_11 * theta
            + static_cast<T>(2.0F) * q1_w * q1_x * s0_21 * theta - static_cast<T>(2.0F) * qperp_w * qperp_x * s0_21 * theta
            + static_cast<T>(2.0F) * qperp_y * qperp_z * s0_21 * theta + q1_y * (-(qperp_x * s0_01) - qperp_z * s0_21
                + qperp_x * s1_01 + qperp_z * s1_21 - static_cast<T>(2.0F) * q1_x * s0_01 * theta)
            + q1_z * (static_cast<T>(2.0F) * qperp_z * s0_11 - qperp_y * s0_21 + qperp_w * (-s0_01 + s1_01)
                - static_cast<T>(2.0F) * qperp_z * s1_11 + qperp_y * s1_21 - static_cast<T>(2.0F) * q1_w * s0_01 * theta
                - static_cast<T>(2.0F) * q1_y * s0_21 * theta)
            , -(q1_x * qperp_y * s0_02) - q1_w * qperp_z * s0_02 + static_cast<T>(2.0F) * q1_x * qperp_x * s0_12
            + q1_x * qperp_w * s0_22 + q1_w * qperp_x * s0_22 + q1_x * qperp_y * s1_02 + q1_w * qperp_z * s1_02
            - static_cast<T>(2.0F) * q1_x * qperp_x * s1_12 - q1_x * qperp_w * s1_22 - q1_w * qperp_x * s1_22
            + static_cast<T>(2.0F) * qperp_x * qperp_y * s0_02 * theta + static_cast<T>(2.0F) * qperp_w * qperp_z * s0_02 * theta
            + static_cast<T>(2.0F) * q1_x * q1_x * s0_12 * theta + static_cast<T>(2.0F) * q1_z * q1_z * s0_12 * theta
            - static_cast<T>(2.0F) * qperp_x * qperp_x * s0_12 * theta - static_cast<T>(2.0F) * qperp_z * qperp_z * s0_12 * theta
            + static_cast<T>(2.0F) * q1_w * q1_x * s0_22 * theta - static_cast<T>(2.0F) * qperp_w * qperp_x * s0_22 * theta
            + static_cast<T>(2.0F) * qperp_y * qperp_z * s0_22 * theta + q1_y * (-(qperp_x * s0_02) - qperp_z * s0_22
                + qperp_x * s1_02 + qperp_z * s1_22 - static_cast<T>(2.0F) * q1_x * s0_02 * theta)
            + q1_z * (static_cast<T>(2.0F) * qperp_z * s0_12 - qperp_y * s0_22 + qperp_w * (-s0_02 + s1_02)
                - static_cast<T>(2.0F) * qperp_z * s1_12 + qperp_y * s1_22 - static_cast<T>(2.0F) * q1_w * s0_02 * theta
                - static_cast<T>(2.0F) * q1_y * s0_22 * theta));

        c5[1] = DerivativeTerm(static_cast<T>(0.0F), -static_cast<T>(2.0F) * (qperp_x * qperp_y * s0_00 + qperp_w * qperp_z * s0_00
            + q1_z * q1_z * s0_10 - qperp_x * qperp_x * s0_10 - qperp_z * qperp_z * s0_10 - q1_y * q1_z * s0_20
            - qperp_w * qperp_x * s0_20 + qperp_y * qperp_z * s0_20 - qperp_x * qperp_y * s1_00 - qperp_w * qperp_z * s1_00
            + q1_w * q1_z * (-s0_00 + s1_00) + q1_x * q1_x * (s0_10 - s1_10) - q1_z * q1_z * s1_10 + qperp_x * qperp_x * s1_10
            + qperp_z * qperp_z * s1_10 + q1_x * (q1_y * (-s0_00 + s1_00) + q1_w * (s0_20 - s1_20)) + q1_y * q1_z * s1_20
            + qperp_w * qperp_x * s1_20 - qperp_y * qperp_z * s1_20) * theta
            , -static_cast<T>(2.0F) * (qperp_x * qperp_y * s0_01 + qperp_w * qperp_z * s0_01 + q1_z * q1_z * s0_11
                - qperp_x * qperp_x * s0_11 - qperp_z * qperp_z * s0_11 - q1_y * q1_z * s0_21 - qperp_w * qperp_x * s0_21
                + qperp_y * qperp_z * s0_21 - qperp_x * qperp_y * s1_01 - qperp_w * qperp_z * s1_01 + q1_w * q1_z * (-s0_01 + s1_01)
                + q1_x * q1_x * (s0_11 - s1_11) - q1_z * q1_z * s1_11 + qperp_x * qperp_x * s1_11 + qperp_z * qperp_z * s1_11
                + q1_x * (q1_y * (-s0_01 + s1_01) + q1_w * (s0_21 - s1_21)) + q1_y * q1_z * s1_21 + qperp_w * qperp_x * s1_21
                - qperp_y * qperp_z * s1_21) * theta
            , -static_cast<T>(2.0F) * (qperp_x * qperp_y * s0_02 + qperp_w * qperp_z * s0_02 + q1_z * q1_z * s0_12
                - qperp_x * qperp_x * s0_12 - qperp_z * qperp_z * s0_12 - q1_y * q1_z * s0_22 - qperp_w * qperp_x * s0_22
                + qperp_y * qperp_z * s0_22 - qperp_x * qperp_y * s1_02 - qperp_w * qperp_z * s1_02 + q1_w * q1_z * (-s0_02 + s1_02)
                + q1_x * q1_x * (s0_12 - s1_12) - q1_z * q1_z * s1_12 + qperp_x * qperp_x * s1_12 + qperp_z * qperp_z * s1_12
                + q1_x * (q1_y * (-s0_02 + s1_02) + q1_w * (s0_22 - s1_22)) + q1_y * q1_z * s1_22 + qperp_w * qperp_x * s1_22
                - qperp_y * qperp_z * s1_22) * theta);

        c1[2] = DerivativeTerm(-t0_z + t1_z, (qperp_w * qperp_y * s0_00 - qperp_x * qperp_z * s0_00
            - q1_y * q1_z * s0_10 - qperp_w * qperp_x * s0_10 - qperp_y * qperp_z * s0_10 - s0_20 + q1_y * q1_y * s0_20
            + qperp_x * qperp_x * s0_20 + qperp_y * qperp_y * s0_20 - qperp_w * qperp_y * s1_00 + qperp_x * qperp_z * s1_00
            + q1_x * q1_z * (-s0_00 + s1_00) + q1_y * q1_z * s1_10 + qperp_w * qperp_x * s1_10 + qperp_y * qperp_z * s1_10
            + q1_w * (q1_y * (s0_00 - s1_00) + q1_x * (-s0_10 + s1_10)) + q1_x * q1_x * (s0_20 - s1_20) + s1_20 - q1_y * q1_y * s1_20
            - qperp_x * qperp_x * s1_20 - qperp_y * qperp_y * s1_20)
            , (qperp_w * qperp_y * s0_01 - qperp_x * qperp_z * s0_01 - q1_y * q1_z * s0_11 - qperp_w * qperp_x * s0_11
                - qperp_y * qperp_z * s0_11 - s0_21 + q1_y * q1_y * s0_21 + qperp_x * qperp_x * s0_21 + qperp_y * qperp_y * s0_21
                - qperp_w * qperp_y * s1_01 + qperp_x * qperp_z * s1_01 + q1_x * q1_z * (-s0_01 + s1_01) + q1_y * q1_z * s1_11
                + qperp_w * qperp_x * s1_11 + qperp_y * qperp_z * s1_11 + q1_w * (q1_y * (s0_01 - s1_01) + q1_x * (-s0_11 + s1_11))
                + q1_x * q1_x * (s0_21 - s1_21) + s1_21 - q1_y * q1_y * s1_21 - qperp_x * qperp_x * s1_21 - qperp_y * qperp_y * s1_21)
            , (qperp_w * qperp_y * s0_02 - qperp_x * qperp_z * s0_02 - q1_y * q1_z * s0_12 - qperp_w * qperp_x * s0_12
                - qperp_y * qperp_z * s0_12 - s0_22 + q1_y * q1_y * s0_22 + qperp_x * qperp_x * s0_22 + qperp_y * qperp_y * s0_22
                - qperp_w * qperp_y * s1_02 + qperp_x * qperp_z * s1_02 + q1_x * q1_z * (-s0_02 + s1_02) + q1_y * q1_z * s1_12
                + qperp_w * qperp_x * s1_12 + qperp_y * qperp_z * s1_12 + q1_w * (q1_y * (s0_02 - s1_02) + q1_x * (-s0_12 + s1_12))
                + q1_x * q1_x * (s0_22 - s1_22) + s1_22 - q1_y * q1_y * s1_22 - qperp_x * qperp_x * s1_22 - qperp_y * qperp_y * s1_22));

        c2[2] = DerivativeTerm(static_cast<T>(0.0F), (q1_w * q1_y * s0_00 - q1_x * q1_z * s0_00 - qperp_w * qperp_y * s0_00
            + qperp_x * qperp_z * s0_00 - q1_w * q1_x * s0_10 - q1_y * q1_z * s0_10 + qperp_w * qperp_x * s0_10
            + qperp_y * qperp_z * s0_10 + q1_x * q1_x * s0_20 + q1_y * q1_y * s0_20 - qperp_x * qperp_x * s0_20
            - qperp_y * qperp_y * s0_20 - q1_w * q1_y * s1_00 + q1_x * q1_z * s1_00 + qperp_w * qperp_y * s1_00
            - qperp_x * qperp_z * s1_00 + q1_w * q1_x * s1_10 + q1_y * q1_z * s1_10 - qperp_w * qperp_x * s1_10
            - qperp_y * qperp_z * s1_10 - q1_x * q1_x * s1_20 - q1_y * q1_y * s1_20 + qperp_x * qperp_x * s1_20
            + qperp_y * qperp_y * s1_20 - static_cast<T>(2.0F) * q1_y * qperp_w * s0_00 * theta
            + static_cast<T>(2.0F) * q1_z * qperp_x * s0_00 * theta
            - static_cast<T>(2.0F) * q1_w * qperp_y * s0_00 * theta + static_cast<T>(2.0F) * q1_x * qperp_z * s0_00 * theta
            + static_cast<T>(2.0F) * q1_x * qperp_w * s0_10 * theta + static_cast<T>(2.0F) * q1_w * qperp_x * s0_10 * theta
            + static_cast<T>(2.0F) * q1_z * qperp_y * s0_10 * theta + static_cast<T>(2.0F) * q1_y * qperp_z * s0_10 * theta
            - static_cast<T>(4.0F) * q1_x * qperp_x * s0_20 * theta
            - static_cast<T>(4.0F) * q1_y * qperp_y * s0_20 * theta)
            , (q1_w * q1_y * s0_01 - q1_x * q1_z * s0_01 - qperp_w * qperp_y * s0_01 + qperp_x * qperp_z * s0_01
                - q1_w * q1_x * s0_11 - q1_y * q1_z * s0_11 + qperp_w * qperp_x * s0_11 + qperp_y * qperp_z * s0_11
                + q1_x * q1_x * s0_21 + q1_y * q1_y * s0_21 - qperp_x * qperp_x * s0_21 - qperp_y * qperp_y * s0_21
                - q1_w * q1_y * s1_01 + q1_x * q1_z * s1_01 + qperp_w * qperp_y * s1_01 - qperp_x * qperp_z * s1_01
                + q1_w * q1_x * s1_11 + q1_y * q1_z * s1_11 - qperp_w * qperp_x * s1_11 - qperp_y * qperp_z * s1_11
                - q1_x * q1_x * s1_21 - q1_y * q1_y * s1_21 + qperp_x * qperp_x * s1_21 + qperp_y * qperp_y * s1_21
                - static_cast<T>(2.0F) * q1_y * qperp_w * s0_01 * theta + static_cast<T>(2.0F) * q1_z * qperp_x * s0_01 * theta
                - static_cast<T>(2.0F) * q1_w * qperp_y * s0_01 * theta + static_cast<T>(2.0F) * q1_x * qperp_z * s0_01 * theta
                + static_cast<T>(2.0F) * q1_x * qperp_w * s0_11 * theta + static_cast<T>(2.0F) * q1_w * qperp_x * s0_11 * theta
                + static_cast<T>(2.0F) * q1_z * qperp_y * s0_11 * theta + static_cast<T>(2.0F) * q1_y * qperp_z * s0_11 * theta
                - static_cast<T>(4.0F) * q1_x * qperp_x * s0_21 * theta - 4 * q1_y * qperp_y * s0_21 * theta)
            , (q1_w * q1_y * s0_02 - q1_x * q1_z * s0_02 - qperp_w * qperp_y * s0_02 + qperp_x * qperp_z * s0_02
                - q1_w * q1_x * s0_12 - q1_y * q1_z * s0_12 + qperp_w * qperp_x * s0_12 + qperp_y * qperp_z * s0_12
                + q1_x * q1_x * s0_22 + q1_y * q1_y * s0_22 - qperp_x * qperp_x * s0_22 - qperp_y * qperp_y * s0_22
                - q1_w * q1_y * s1_02 + q1_x * q1_z * s1_02 + qperp_w * qperp_y * s1_02 - qperp_x * qperp_z * s1_02
                + q1_w * q1_x * s1_12 + q1_y * q1_z * s1_12 - qperp_w * qperp_x * s1_12 - qperp_y * qperp_z * s1_12
                - q1_x * q1_x * s1_22 - q1_y * q1_y * s1_22 + qperp_x * qperp_x * s1_22 + qperp_y * qperp_y * s1_22
                - static_cast<T>(2.0F) * q1_y * qperp_w * s0_02 * theta + static_cast<T>(2.0F) * q1_z * qperp_x * s0_02 * theta
                - static_cast<T>(2.0F) * q1_w * qperp_y * s0_02 * theta + static_cast<T>(2.0F) * q1_x * qperp_z * s0_02 * theta
                + static_cast<T>(2.0F) * q1_x * qperp_w * s0_12 * theta + static_cast<T>(2.0F) * q1_w * qperp_x * s0_12 * theta
                + static_cast<T>(2.0F) * q1_z * qperp_y * s0_12 * theta
                + static_cast<T>(2.0F) * q1_y * qperp_z * s0_12 * theta - static_cast<T>(4.0F) * q1_x * qperp_x * s0_22 * theta
                - static_cast<T>(4.0F) * q1_y * qperp_y * s0_22 * theta));

        c3[2] = DerivativeTerm(static_cast<T>(0.0F), -static_cast<T>(2.0F) * (-(q1_w * qperp_y * s0_00) + q1_x * qperp_z * s0_00
            + q1_x * qperp_w * s0_10 + q1_w * qperp_x * s0_10 - static_cast<T>(2.0F) * q1_x * qperp_x * s0_20 + q1_w * qperp_y * s1_00
            - q1_x * qperp_z * s1_00 - q1_x * qperp_w * s1_10 - q1_w * qperp_x * s1_10 + q1_z * (qperp_x * s0_00
                + qperp_y * s0_10 - qperp_x * s1_00 - qperp_y * s1_10) + static_cast<T>(2.0F) * q1_x * qperp_x * s1_20
            + q1_y * (qperp_z * s0_10 - static_cast<T>(2.0F) * qperp_y * s0_20 + qperp_w * (-s0_00 + s1_00) - qperp_z * s1_10
                + static_cast<T>(2.0F) * qperp_y * s1_20)) * theta
            , -static_cast<T>(2.0F) * (-(q1_w * qperp_y * s0_01) + q1_x * qperp_z * s0_01 + q1_x * qperp_w * s0_11
                + q1_w * qperp_x * s0_11 - static_cast<T>(2.0F) * q1_x * qperp_x * s0_21 + q1_w * qperp_y * s1_01
                - q1_x * qperp_z * s1_01 - q1_x * qperp_w * s1_11 - q1_w * qperp_x * s1_11 + q1_z * (qperp_x * s0_01
                    + qperp_y * s0_11 - qperp_x * s1_01 - qperp_y * s1_11) + static_cast<T>(2.0F) * q1_x * qperp_x * s1_21
                + q1_y * (qperp_z * s0_11 - static_cast<T>(2.0F) * qperp_y * s0_21 + qperp_w * (-s0_01 + s1_01) - qperp_z * s1_11
                    + static_cast<T>(2.0F) * qperp_y * s1_21)) * theta
            , -static_cast<T>(2.0F) * (-(q1_w * qperp_y * s0_02) + q1_x * qperp_z * s0_02 + q1_x * qperp_w * s0_12 + q1_w * qperp_x * s0_12
                - static_cast<T>(2.0F) * q1_x * qperp_x * s0_22 + q1_w * qperp_y * s1_02 - q1_x * qperp_z * s1_02 - q1_x * qperp_w * s1_12
                - q1_w * qperp_x * s1_12 + q1_z * (qperp_x * s0_02 + qperp_y * s0_12 - qperp_x * s1_02 - qperp_y * s1_12)
                + static_cast<T>(2.0F) * q1_x * qperp_x * s1_22 + q1_y * (qperp_z * s0_12 - static_cast<T>(2.0F) * qperp_y * s0_22
                    + qperp_w * (-s0_02 + s1_02) - qperp_z * s1_12 + static_cast<T>(2.0F) * qperp_y * s1_22)) * theta);

        c4[2] = DerivativeTerm(static_cast<T>(0.0F), q1_w * qperp_y * s0_00 - q1_x * qperp_z * s0_00 - q1_x * qperp_w * s0_10
            - q1_w * qperp_x * s0_10 + static_cast<T>(2.0F) * q1_x * qperp_x * s0_20 - q1_w * qperp_y * s1_00 + q1_x * qperp_z * s1_00
            + q1_x * qperp_w * s1_10 + q1_w * qperp_x * s1_10 - static_cast<T>(2.0F) * q1_x * qperp_x * s1_20
            - static_cast<T>(2.0F) * qperp_w * qperp_y * s0_00 * theta + static_cast<T>(2.0F) * qperp_x * qperp_z * s0_00 * theta
            - static_cast<T>(2.0F) * q1_w * q1_x * s0_10 * theta + static_cast<T>(2.0F) * qperp_w * qperp_x * s0_10 * theta
            + static_cast<T>(2.0F) * qperp_y * qperp_z * s0_10 * theta + static_cast<T>(2.0F) * q1_x * q1_x * s0_20 * theta
            + static_cast<T>(2.0F) * q1_y * q1_y * s0_20 * theta - static_cast<T>(2.0F) * qperp_x * qperp_x * s0_20 * theta
            - static_cast<T>(2.0F) * qperp_y * qperp_y * s0_20 * theta + q1_z * (-(qperp_x * s0_00) - qperp_y * s0_10
                + qperp_x * s1_00 + qperp_y * s1_10 - static_cast<T>(2.0F) * q1_x * s0_00 * theta) + q1_y * (-(qperp_z * s0_10)
                    + static_cast<T>(2.0F) * qperp_y * s0_20 + qperp_w * (s0_00 - s1_00) + qperp_z * s1_10
                    - static_cast<T>(2.0F) * qperp_y * s1_20
                    + static_cast<T>(2.0F) * q1_w * s0_00 * theta - static_cast<T>(2.0F) * q1_z * s0_10 * theta)
            , q1_w * qperp_y * s0_01 - q1_x * qperp_z * s0_01 - q1_x * qperp_w * s0_11 - q1_w * qperp_x * s0_11
            + static_cast<T>(2.0F) * q1_x * qperp_x * s0_21 - q1_w * qperp_y * s1_01 + q1_x * qperp_z * s1_01 + q1_x * qperp_w * s1_11
            + q1_w * qperp_x * s1_11 - static_cast<T>(2.0F) * q1_x * qperp_x * s1_21
            - static_cast<T>(2.0F) * qperp_w * qperp_y * s0_01 * theta
            + static_cast<T>(2.0F) * qperp_x * qperp_z * s0_01 * theta - static_cast<T>(2.0F) * q1_w * q1_x * s0_11 * theta
            + static_cast<T>(2.0F) * qperp_w * qperp_x * s0_11 * theta + static_cast<T>(2.0F) * qperp_y * qperp_z * s0_11 * theta
            + static_cast<T>(2.0F) * q1_x * q1_x * s0_21 * theta + static_cast<T>(2.0F) * q1_y * q1_y * s0_21 * theta
            - static_cast<T>(2.0F) * qperp_x * qperp_x * s0_21 * theta - static_cast<T>(2.0F) * qperp_y * qperp_y * s0_21 * theta
            + q1_z * (-(qperp_x * s0_01) - qperp_y * s0_11 + qperp_x * s1_01 + qperp_y * s1_11 - static_cast<T>(2.0F) * q1_x * s0_01 * theta)
            + q1_y * (-(qperp_z * s0_11) + static_cast<T>(2.0F) * qperp_y * s0_21 + qperp_w * (s0_01 - s1_01) + qperp_z * s1_11
                - static_cast<T>(2.0F) * qperp_y * s1_21 + static_cast<T>(2.0F) * q1_w * s0_01 * theta
                - static_cast<T>(2.0F) * q1_z * s0_11 * theta)
            , q1_w * qperp_y * s0_02 - q1_x * qperp_z * s0_02 - q1_x * qperp_w * s0_12 - q1_w * qperp_x * s0_12
            + static_cast<T>(2.0F) * q1_x * qperp_x * s0_22 - q1_w * qperp_y * s1_02 + q1_x * qperp_z * s1_02 + q1_x * qperp_w * s1_12
            + q1_w * qperp_x * s1_12 - static_cast<T>(2.0F) * q1_x * qperp_x * s1_22
            - static_cast<T>(2.0F) * qperp_w * qperp_y * s0_02 * theta
            + static_cast<T>(2.0F) * qperp_x * qperp_z * s0_02 * theta - static_cast<T>(2.0F) * q1_w * q1_x * s0_12 * theta
            + static_cast<T>(2.0F) * qperp_w * qperp_x * s0_12 * theta + static_cast<T>(2.0F) * qperp_y * qperp_z * s0_12 * theta
            + static_cast<T>(2.0F) * q1_x * q1_x * s0_22 * theta + static_cast<T>(2.0F) * q1_y * q1_y * s0_22 * theta
            - static_cast<T>(2.0F) * qperp_x * qperp_x * s0_22 * theta - static_cast<T>(2.0F) * qperp_y * qperp_y * s0_22 * theta
            + q1_z * (-(qperp_x * s0_02) - qperp_y * s0_12 + qperp_x * s1_02 + qperp_y * s1_12
                - static_cast<T>(2.0F) * q1_x * s0_02 * theta)
            + q1_y * (-(qperp_z * s0_12) + static_cast<T>(2.0F) * qperp_y * s0_22 + qperp_w * (s0_02 - s1_02) + qperp_z * s1_12
                - static_cast<T>(2.0F) * qperp_y * s1_22 + static_cast<T>(2.0F) * q1_w * s0_02 * theta
                - static_cast<T>(2.0F) * q1_z * s0_12 * theta));

        c5[2] = DerivativeTerm(static_cast<T>(0.0F), static_cast<T>(2.0F) * (qperp_w * qperp_y * s0_00 - qperp_x * qperp_z * s0_00
            + q1_y * q1_z * s0_10 - qperp_w * qperp_x * s0_10 - qperp_y * qperp_z * s0_10 - q1_y * q1_y * s0_20
            + qperp_x * qperp_x * s0_20 + qperp_y * qperp_y * s0_20 + q1_x * q1_z * (s0_00 - s1_00)
            - qperp_w * qperp_y * s1_00 + qperp_x * qperp_z * s1_00 + q1_w * (q1_y * (-s0_00 + s1_00)
                + q1_x * (s0_10 - s1_10)) - q1_y * q1_z * s1_10 + qperp_w * qperp_x * s1_10 + qperp_y * qperp_z * s1_10
            + q1_y * q1_y * s1_20 - qperp_x * qperp_x * s1_20 - qperp_y * qperp_y * s1_20
            + q1_x * q1_x * (-s0_20 + s1_20)) * theta
            , static_cast<T>(2.0F) * (qperp_w * qperp_y * s0_01 - qperp_x * qperp_z * s0_01 + q1_y * q1_z * s0_11
                - qperp_w * qperp_x * s0_11 - qperp_y * qperp_z * s0_11 - q1_y * q1_y * s0_21 + qperp_x * qperp_x * s0_21
                + qperp_y * qperp_y * s0_21 + q1_x * q1_z * (s0_01 - s1_01) - qperp_w * qperp_y * s1_01
                + qperp_x * qperp_z * s1_01 + q1_w * (q1_y * (-s0_01 + s1_01) + q1_x * (s0_11 - s1_11))
                - q1_y * q1_z * s1_11 + qperp_w * qperp_x * s1_11 + qperp_y * qperp_z * s1_11 + q1_y * q1_y * s1_21
                - qperp_x * qperp_x * s1_21 - qperp_y * qperp_y * s1_21 + q1_x * q1_x * (-s0_21 + s1_21)) * theta
            , static_cast<T>(2.0F) * (qperp_w * qperp_y * s0_02 - qperp_x * qperp_z * s0_02 + q1_y * q1_z * s0_12
                - qperp_w * qperp_x * s0_12
                - qperp_y * qperp_z * s0_12 - q1_y * q1_y * s0_22 + qperp_x * qperp_x * s0_22 + qperp_y * qperp_y * s0_22
                + q1_x * q1_z * (s0_02 - s1_02) - qperp_w * qperp_y * s1_02 + qperp_x * qperp_z * s1_02
                + q1_w * (q1_y * (-s0_02 + s1_02) + q1_x * (s0_12 - s1_12)) - q1_y * q1_z * s1_12 + qperp_w * qperp_x * s1_12
                + qperp_y * qperp_z * s1_12 + q1_y * q1_y * s1_22 - qperp_x * qperp_x * s1_22 - qperp_y * qperp_y * s1_22
                + q1_x * q1_x * (-s0_22 + s1_22)) * theta);
    }
}


template<typename T>
void AnimatedTransform<T>::Decompose(const Mat4<T> &m, Vec3<T> *translation, Quaternion<T> *rotation_quat, Mat4<T> *scale)
{
    /// Extract translation _T_ from transformation matrix
    translation->x = m[0][3];
    translation->y = m[1][3];
    translation->z = m[2][3];

    /// Compute new transformation matrix _M_ without translation
    Mat4<T> M = m;
    for (int i = 0; i < 3; ++i)
    {
        M[i][3] = M[3][i] = static_cast<T>(0.0F);
    }
    M[3][3] = static_cast<T>(1.0F);

    /// Extract rotation _R_ from transformation matrix
    T norm;
    int count = 0;
    Mat4<T> rotation = M;
    do
    {
        /// Compute next matrix _Rnext_ in series
        Mat4<T> Rnext;
        Mat4<T> Rit = Inverse(Transpose(rotation));
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                Rnext[i][j] = static_cast<T>(0.5F) * (rotation[i][j] + Rit[i][j]);
            }
        }

        /// Compute norm of difference between _R_ and _Rnext_
        norm = static_cast<T>(0.0F);
        for (int i = 0; i < 3; ++i)
        {
            T n = std::abs(rotation[i][0] - Rnext[i][0])
                + std::abs(rotation[i][1] - Rnext[i][1])
                + std::abs(rotation[i][2] - Rnext[i][2]);
            norm = (std::max)(norm, n);
        }
        rotation = Rnext;
    }
    while (++count < 100 && norm > static_cast<T>(0.0001F));
    /// XXX TODO FIXME deal with flip...
    *rotation_quat = Quaternion<T>(rotation);

    /// Compute scale _S_ using rotation and original matrix
    *scale = Mul(Inverse(rotation), M);
}

template<typename T>
void AnimatedTransform<T>::Interpolate(T time, Transform<T> *t) const
{
    /// Handle boundary conditions for matrix interpolation
    if (!is_actually_animated || time <= start_time)
    {
        *t = *p_start_transform;
        return;
    }
    if (time >= end_time)
    {
        *t = *p_end_transform;
        return;
    }
    T dt = (time - start_time) / (end_time - start_time);
    /// Interpolate translation at _dt_
    Vec3<T> trans = (static_cast<T>(1.0F) - dt) * translations[0] + dt * translations[1];

    /// Interpolate rotation at _dt_
    Quaternion<T> rotate = Slerp(dt, rotations[0], rotations[1]);

    /// Interpolate scale at _dt_
    Mat4<T> scale;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            scale[i][j] = Lerp(dt, (scales[0])[i][j], (scales[1])[i][j]);
        }
    }

    /// Compute interpolated matrix as product of interpolated components
    /// TOOO
    //*t = Translate(trans) * rotate.ToTransform() * Transform(scale);
}


template<typename T>
Ray<T> AnimatedTransform<T>::operator()(const Ray<T> &r) const
{
    if (!is_actually_animated || r.time <= start_time) return (*p_start_transform)(r);
    else if (r.time >= end_time) return (*p_end_transform)(r);
    else
    {
        Transform<T> t;
        Interpolate(r.time, &t);
        return t(r);
    }
}

template<typename T>
RayDifferential<T> AnimatedTransform<T>::operator()(const RayDifferential<T> &r) const
{
    if (!is_actually_animated || r.time <= start_time) return (*p_start_transform)(r);
    else if (r.time >= end_time) return (*p_end_transform)(r);
    else
    {
        Transform<T> t;
        Interpolate(r.time, &t);
        return t(r);
    }
}

template<typename T>
Vec3<T> AnimatedTransform<T>::operator()(T time, const Vec3<T> &v) const
{
    if (!is_actually_animated || time <= start_time) return (*p_start_transform)(v);
    else if (time >= end_time) return (*p_end_transform)(v);
    else
    {
        Transform<T> t;
        Interpolate(time, &t);
        return t(v);
    }
}


template<typename T>
Bounds3<T> AnimatedTransform<T>::MotionBounds(const Bounds3<T> &b) const
{
    if (!is_actually_animated) return (*p_start_transform)(b);
    if (!has_rotation) return Union((*p_start_transform)(b), (*p_end_transform)(b));

    /// Return motion bounds accounting for animated rotation
    Bounds3<T> bounds;
    for (int corner = 0; corner < 8; ++corner)
    {
        bounds = Union(bounds, BoundPointMotion(b.Corner(corner)));
    }
    return bounds;
}

template<typename T>
Bounds3<T> AnimatedTransform<T>::BoundPointMotion(const Vec3<T> &p) const
{
    if (!is_actually_animated) return Bounds3<T>((*p_start_transform)(p));

    Bounds3<T> bounds((*p_start_transform)(p), (*p_end_transform)(p));
    T cos_theta = Dot(rotations[0], rotations[1]);
    T theta = std::acos(Clamp(cos_theta, -1, 1));
    for (int c = 0; c < 3; ++c)
    {
        /// Find any motion derivative zeros for the component _c_
        T zeros[8];
        int zeros_number = 0;
        /// TODO
        /*
        IntervalFindZeros(c1[c].Eval(p), c2[c].Eval(p), c3[c].Eval(p),
            c4[c].Eval(p), c5[c].Eval(p), theta, Interval(0., 1.),
            zeros, &zeros_number);
            */
        CHECK_LE(zeros_number, sizeof(zeros) / sizeof(zeros[0]));

        /// Expand bounding box for any motion derivative zeros found
        for (int i = 0; i < zeros_number; ++i)
        {
            Vec3<T> pz = (*this)(Lerp(zeros[i], start_time, end_time), p);
            bounds = Union(bounds, pz);
        }
    }

    return bounds;
}


}
}