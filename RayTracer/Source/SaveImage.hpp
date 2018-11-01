#pragma once

#include "pch.h"
#include <cstdlib>
#include <string>
#include <vector>
#include <ctime>
#include "Color.hpp"

using namespace std;

namespace Utilities {
    namespace Tools {
#ifdef PPM_FORMAT
        inline
        static void pixel_write(FILE *fp, const unsigned char r, const unsigned char g, const unsigned char b) {
            static unsigned char color[3];
            color[0] = r & 255;
            color[1] = g & 255;
            color[2] = b & 255;
            fwrite(color, 1, 3, fp);
        }
#endif // PPM_FORMAT

        static void SaveImage(vector<vector<Color<unsigned char>>> ss) {
            size_t col = ss.size();
            size_t row = ss[0].size();
#ifdef PPM_FORMAT
            time_t pTime; 
            time(&pTime);
            struct tm *p = localtime(&pTime);
            string curTimeStr = to_string(1900 + p->tm_year) + "." + to_string(1 + p->tm_mon) + "." + to_string(p->tm_mday) + "_" + 
                to_string(p->tm_hour) + "." + to_string(p->tm_min) + "." + to_string(p->tm_sec);
            
            FILE *fp = fopen((".\\Resource\\PPM\\" + curTimeStr +".ppm").c_str(), "wb");
            fprintf(fp, "P6\n%d %d\n255\n", (int)row, (int)col);

            for (size_t r = 0; r < row; ++r) {
                for (size_t c = 0; c < col; ++c) {
                    auto tmp = ss[c][r];
                    pixel_write(fp, tmp.GetR(), tmp.GetG(), tmp.GetB());
                }
            }

            fclose(fp);
#endif // PPM_FORMAT
        }
    }
}