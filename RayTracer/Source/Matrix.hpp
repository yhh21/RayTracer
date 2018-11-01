#pragma once

#include "pch.h"

namespace Utilities{
    namespace Math {
        template<class T = double>
        class Matrix {
        public:
            size_t GetCol() const { return this->col; }
            size_t GetRow() const { return this->row; }

            template<class T>
            T GetValue(const size_t col, const size_t row) const { return this->mat[col][row]; }
            template<class T>
            void SetValue(const size_t col, const size_t row, const T value) { this->mat[col][row] = value; }


            Matrix(const size_t col, const size_t row) {
                this->col = col;
                this->row = row;

                this->mat = new T*[col];
                for (size_t i = 0; i < col; ++i) {
                    this->mat[i] = new T[row]();
                }
            }

            Matrix(const Matrix<T> &matrix) {
                Clone(matrix);
            }

            ~Matrix() {
                for (size_t i = 0; i < this->col; ++i) delete mat[i];
                delete mat;
            }


            template<class T2>
            void Clone(const Matrix<T2> &matrix) {
                for (size_t c = 0; c < this->col; ++c) delete mat[c];
                delete mat;

                this->col = matrix.GetCol();
                this->row = matrix.GetRow();

                this->mat = new T*[col];
                for (size_t c = 0; c < col; ++c) {
                    this->mat[c] = new T[row]();
                }

                for (size_t c = 0; c < col; ++c) {
                    for (size_t r = 0; r < row; ++r) {
                        this->SetValue<T>(c, r, matrix.GetValue<T2>(c, r));
                    }
                }
            }

#ifdef DEBUG
            void Print() const {
                for (size_t c = 0; c < col; ++c) {
                    for (size_t r = 0; r < row; ++r) {
                        if (r == 0) cout << "[ ";
                        cout << this->GetValue<T>(c, r);
                        cout << r != row - 1 ? " " : " ]";
                    }
                }
                cout << "\n";
            }
#endif // DEBUG
        private:
            T **mat;
            size_t col, row;
        };


        // Extend
        template<class T = double>
        class Matrix2x2 : public Matrix<T> {
        public:
            Matrix2x2() : Matrix<T>(2, 2) {}
        };

        template<class T = double>
        class Matrix3x3 : public Matrix<T> {
        public:
            Matrix3x3() : Matrix<T>(3, 3) {}
        };

        template<class T = double>
        class Matrix4x4 : public Matrix<T> {
        public:
            Matrix4x4() : Matrix<T>(4, 4) {}
        };
    }


#define DMatrix2x2 Matrix2x2<double>
#define DMatrix3x3 Matrix3x3<double>
#define DMatrix4x4 Matrix4x4<double>
}