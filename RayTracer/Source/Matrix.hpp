#pragma once

#include "pch.h"

namespace Utilities{
    namespace Math {
        template<class T = double>
        class Matrix {
        public:
            Matrix(const size_t col, const size_t row) {
                this->col = col;
                this->row = row;

                this->mat = new T*[col];
                for (size_t i = 0; i < col; ++i) {
                    this->mat[i] = new T[row]();
                }

                //for (size_t i = 0; i < col; ++i)
                //    for (size_t j = 0; j < row; ++j)
                //        this->mat[i][j] = T(0);
            }

            ~Matrix() {
                for (size_t i = 0; i < this->col; ++i) delete mat[i];
                delete mat;
            }


            size_t GetCol() const { return this->col; }

            size_t GetRow() const { return this->row; }

            template<class T>
            T GetValue(const size_t col, const size_t row) const { return this->mat[col][row]; }

            template<class T>
            void SetValue(const size_t col, const size_t row, const T value) { this->mat[col][row] = value; }
        private:
            T **mat;
            size_t col, row;
        };


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
}