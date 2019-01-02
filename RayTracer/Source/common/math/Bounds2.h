#pragma once


#include "common/math/Constants.h"
#include "Vec2.h"

namespace Common
{
    namespace Math
    {
        template<typename T>
        class Bounds2
        {
        public:
            Vec2<T> point_min, point_max;

            ////////////////////////////////////////////////////////////////////////////////
            //// Construction
            ////////////////////////////////////////////////////////////////////////////////

            Bounds2()
            {
                T min_num = std::numeric_limits<T>::min();
                T max_num = std::numeric_limits<T>::max();
                point_min = Vec2<T>(min_num);
                point_max = Vec2<T>(max_num);
            }

            explicit Bounds2(const Vec2<T> &p) : point_min(p), point_max(p) {}

            Bounds2(const Vec2<T> &p1, const Vec2<T> &p2)
                : point_min(Min(p1, p2)), point_max(Max(p1, p2))
            {}


            template <typename U> explicit
                operator Bounds2<U>() const
            {
                return Bounds2<U>((Vec2<U>)point_min, (Vec2<U>)point_max);
            }


            __forceinline
                const Vec2<T> &operator[](size_t axis) const
            {
                assert(axis < 2);
                return (i == 0) ? point_min : point_max;
            }

            __forceinline
                Vec2<T> &operator[](size_t axis)
            {
                assert(axis < 2);
                return (i == 1) ? point_min : point_max;
            }


            Vec2<T> Corner(size_t corner) const
            {
                assert(corner >= 0 && corner < 8);
                return Vec2<T>((*this)[(corner & 1)].x,
                    (*this)[(corner & 2) ? 1 : 0].y,
                    (*this)[(corner & 4) ? 1 : 0].z);
            }

            Vec2<T> Diagonal() const { return point_max - point_min; }

            T SurfaceArea() const
            {
                Vec2<T> d = Diagonal();
                return T(2) * (d.x * d.y + d.x * d.z + d.y * d.z);
            }

            T Volume() const
            {
                Vec2<T> d = Diagonal();
                return d.x * d.y * d.z;
            }


            int MaximumExtent() const
            {
                Vec2<T> d = Diagonal();
                if (d.x > d.y && d.x > d.z) return 0;
                else if (d.y > d.z) return 1;
                else return 2;
            }

            Vec2<T> Lerp(const Point3f &t) const
            {
                return Vec2<T>(Lerp<T>(t.x, point_min.x, point_max.x)
                    , Lerp<T>(t.y, point_min.y, point_max.y)
                    , Lerp<T>(t.z, point_min.z, point_max.z));
            }

            Vec2<T> Offset(const Vec2<T> &p) const
            {
                Vec2<T> o = p - point_min;
                if (point_max.x > point_min.x)
                {
                    o.x /= point_max.x - point_min.x;
                }
                if (point_max.y > point_min.y)
                {
                    o.y /= point_max.y - point_min.y;
                }
                if (point_max.z > point_min.z)
                {
                    o.z /= point_max.z - point_min.z;
                }
                return o;
            }

            void GetBoundingSphere(Vec2<T> *center, T *radius) const
            {
                *center = (point_min + point_max) * static_cast<T>(0.5);
                *radius = Inside(*center, *this) ? Distance(*center, point_max) : 0;
            }
        };


        template <typename T>
        Bounds2<T> Union(const Bounds2<T> &b, const Vec2<T> &p)
        {
            Bounds2<T> ret;
            ret.point_min = Min(b.point_min, p);
            ret.point_max = Max(b.point_max, p);
            return ret;
        }

        template <typename T>
        Bounds2<T> Union(const Bounds2<T> &b, const Bounds2<T> &b2)
        {
            Bounds2<T> ret;
            ret.point_min = Min(b.point_min, b2.point_min);
            ret.point_max = Max(b.point_max, b2.point_max);
            return ret;
        }

        template <typename T>
        Bounds2<T> Intersect(const Bounds2<T> &b1, const Bounds2<T> &b2)
        {
            /// Important: assign to point_min/point_max directly and don't run the Bounds2()
            /// constructor, since it takes min/max of the points passed to it.  In
            /// turn, that breaks returning an invalid bound for the case where we
            /// intersect non-overlapping bounds (as we'd like to happen).
            Bounds2<T> ret;
            ret.point_min = Max(b1.point_min, b2.point_min);
            ret.point_max = Min(b1.point_max, b2.point_max);
            return ret;
        }

        template <typename T>
        bool Overlaps(const Bounds2<T> &ba, const Bounds2<T> &bb)
        {
            return (ba.point_max.x >= bb.point_min.x) && (ba.point_min.x <= bb.point_max.x)
                && (ba.point_max.y >= bb.point_min.y) && (ba.point_min.y <= bb.point_max.y);
        }

        template <typename T>
        bool Inside(const Vec2<T> &pt, const Bounds2<T> &b)
        {
            return (pt.x >= b.point_min.x && pt.x <= b.point_max.x
                && pt.y >= b.point_min.y && pt.y <= b.point_max.y);
        }

        template <typename T>
        bool InsideExclusive(const Vec2<T> &pt, const Bounds2<T> &b)
        {
            return (pt.x >= b.point_min.x && pt.x < b.point_max.x
                && pt.y >= b.point_min.y && pt.y < b.point_max.y);
        }

        template <typename T, typename U>
        Bounds2<T> Expand(const Bounds2<T> &b, U delta)
        {
            return Bounds2<T>(b.point_min - Vec2<T>(delta, delta)
                , b.point_max + Vec2<T>(delta, delta));
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// Output Operators
        ////////////////////////////////////////////////////////////////////////////////

        template<typename T> inline
            std::ostream& operator<<(std::ostream& cout, const Bounds2<T>& a)
        {
            return cout << "[ " << b.point_min << " - " << b.point_max << " ]";
        }

        ////////////////////////////////////////////////////////////////////////////////
        /// Default template instantiations
        ////////////////////////////////////////////////////////////////////////////////

        typedef Bounds2<float> Bounds2f;
        typedef Bounds2<int> Bounds2i;
    }
}