#pragma once

#include "pch.h"
#include "Color.h"
#include <vector>

namespace Common
{
    class Window
    {
    public:
        size_t GetCol() const
        {
            return this->col;
        }
        size_t GetRow() const
        {
            return this->row;
        }

        vector<vector<CColor>> GetWindowColors() const
        {
            return this->windowColors;
        }


        Window(const size_t &col, const size_t &row)
        {
            this->col = col;
            this->row = row;
        }
    private:
        size_t col, row;
        vector<vector<CColor>> windowColors;
    };
}