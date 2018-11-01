#pragma once

#include <cmath>

namespace Utilities {
    namespace Math {
        // Base
        template<class T = double>
        class Vector {
        public:
            Vector(const size_t size) {
                this->size = size;
                vec = new T[this->size]();
            }

            Vector(const Vector<T> &v) {
                Clone(v);
            }

            ~Vector() { if (vec != nullptr) delete vec; }


            size_t GetSize() const { return this->size; }

            template<class T>
            T GetValue(const size_t index) const { return this->vec[index]; }

            template<class T>
            void SetValue(const size_t index, const T value) { this->vec[index] = value; }


            // Vector operator
            template<class T2>
            Vector<T>& operator=(const Vector<T2> &v)
            {
                for (size_t i = 0; i < v.GetSize(); ++i) {
                    this->SetValue<T>(i, v.GetValue<T2>(i));
                }
                for (size_t i = v.GetSize(); i < this->GetSize(); ++i) {
                    this->SetValue<T>(i, T2(0));
                }

                return *this;
            }

            Vector<T>& operator-() const {
                Vector<T> res(this->GetSize());

                for (size_t i = 0; i < this->GetSize(); ++i) {
                    res.SetValue<T>(i, -this->GetValue<T>(i));
                }

                return res;
            }

            // Vector operator
            template<class T = double>
            Vector<T> operator+(const Vector<T> &v) const {
                size_t Size1 = this->GetSize();
                size_t Size2 = v.GetSize();
                size_t minSize = Size1 < Size2 ? Size1 : Size2;
                size_t maxSize = Size1 + Size2 - minSize;

                Vector<T> res(maxSize);
                for (size_t i = 0; i < minSize; ++i) {
                    res.SetValue<T>(i, this->GetValue<T>(i) + v.GetValue<T>(i));
                }
                for (size_t i = minSize; i < maxSize; ++i) {
                    res.SetValue<T>(i, Size1 > minSize ? this->GetValue<T>(i) : v.GetValue<T>(i));
                }

                return res;
            }

            template<class T = double>
            Vector<T> operator-(const Vector<T> &v) const {
                size_t Size1 = this->GetSize();
                size_t Size2 = v.GetSize();
                size_t minSize = Size1 < Size2 ? Size1 : Size2;
                size_t maxSize = Size1 + Size2 - minSize;

                Vector<T> res(maxSize);
                for (size_t i = 0; i < minSize; ++i) {
                    res.SetValue<T>(i, this->GetValue<T>(i) - v.GetValue<T>(i));
                }
                for (size_t i = minSize; i < maxSize; ++i) {
                    res.SetValue<T>(i, Size1 > minSize ? this->GetValue<T>(i) : -v.GetValue<T>(i));
                }
                return res;
            }

            template<class T = double>
            Vector<T> operator*(const Vector<T> &v) const {
                size_t Size1 = this->GetSize();
                size_t Size2 = v.GetSize();
                size_t minSize = Size1 < Size2 ? Size1 : Size2;
                size_t maxSize = Size1 + Size2 - minSize;

                Vector<T> res(maxSize);
                for (size_t i = 0; i < minSize; ++i) {
                    res.SetValue<T>(i, this->GetValue<T>(i) * v.GetValue<T>(i));
                }
                for (size_t i = minSize; i < maxSize; ++i) {
                    res.SetValue<T>(i, T(0));
                }

                return res;
            }
            
            template<class T = double>
            Vector<T>& operator*(const T &s) {
                for (size_t i = 0; i < this->GetSize(); ++i) {
                    this->SetValue<T>(i, this->GetValue<T>(i) * s);
                }

                return *this;
            }

            template<class T = double>
            Vector<T>& operator/(const T &s) {
                return *this * (T(1) / s);
            }


            T Dot(const Vector<T> &v) const {
                size_t Size1 = this->GetSize();
                size_t Size2 = v.GetSize();
                size_t minSize = Size1 < Size2 ? Size1 : Size2;

                T res = T(0);
                for (size_t i = 0; i < minSize; ++i) {
                    res += this->GetValue<T>(i) * v.GetValue<T>(i);
                }

                return res;
            }

            T Length() const {
                T res = T(0);
                for (size_t i = 0; i < this->GetSize(); ++i) {
                    res += this->GetValue<T>(i) * this->GetValue<T>(i);
                }

                return sqrt(res);
            }

            Vector<T>& Normalize() {
                return *this / this->Length();
            }



            template<class T2>
            void Clone(const Vector<T2> &v) {
                if (this->vec != nullptr) delete vec;

                this->size = v.GetSize();
                this->vec = new T[this->size]();
                for (size_t i = 0; i < v.GetSize(); ++i) {
                    this->SetValue<T>(i, v.GetValue<T2>(i));
                }
            }

#ifdef DEBUG
            void Print() const {
                for (size_t i = 0; i < size; ++i) {
                    cout << GetValue<T>(i);
                    if (i != size - 1) {
                        cout << " ";
                    }
                }
                cout << "\n";
            }
#endif // DEBUG
        private:
            T *vec = nullptr;
            size_t size;
        };


        // Extend
        template<class T = double>
        class Vector2 : public Vector<T> {
        public:
            Vector2() : Vector<T>(2) {}
            Vector2(const T& a, const T& b) : Vector2() {
                this->SetValue(0, a);
                this->SetValue(1, b);
            }
        };

        template<class T = double>
        class Vector3 : public Vector<T> {
        public:
            Vector3() : Vector<T>(3) {}
            Vector3(const T& a, const T& b, const T& c) : Vector3() {
                this->SetValue(0, a);
                this->SetValue(1, b);
                this->SetValue(2, c);
            }
        };

        template<class T = double>
        class Vector4 : public Vector<T> {
        public:
            Vector4() : Vector<T>(4) {}
            Vector4(const T& a, const T& b, const T& c, const T& d) : Vector4() {
                this->SetValue(0, a);
                this->SetValue(1, b);
                this->SetValue(2, c);
                this->SetValue(3, d);
            }
        };

        // typedef
#define DVector Vector<double>
#define DVector2 Vector2<double>
#define DVector3 Vector3<double>
#define DVector4 Vector4<double>
    }
}