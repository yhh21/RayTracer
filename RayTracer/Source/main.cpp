#include "pch.h"

#include <iostream>

#include <vector>
#include "Matrix.hpp"
#include "Color.hpp"
#include "SaveImage.hpp"
#include "Point.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"


using namespace std;
using namespace Utilities;
using namespace Utilities::Math;
using namespace Objects;


int main() {
    cout << "hello, ray tracer!\n";

    /*
    Utilities::Math::Matrix2x2<double> *m22 = new Utilities::Math::Matrix2x2<double>();
    for (size_t i = 0; i < m22->GetCol(); ++i) {
        for (size_t j = 0; j < m22->GetRow(); ++j) {
            double t = m22->GetValue<double>(i, j);
            t = t;
        }
    }*/

    /*
    Utilities::Math::Vector2<double> *v2 = new Utilities::Math::Vector2<double>();
    for (size_t i = 0; i < v2->GetLength(); ++i) {
        double t = v2->GetValue<double>(i);
        t = t;
    }*/
    
    /*
    Utilities::Math::Point2<double> *v2 = new Utilities::Math::Point2<double>();
    for (size_t i = 0; i < v2->GetLength(); ++i) {
        double t = v2->GetValue<double>(i);
        t = t;
    }//*/


    int col = 300, row = 300;
    Sphere *sphere = new Sphere(CColor(255, 255, 255), DPoint3(row / 2, col / 2, col / 2), col / 4);

    vector<vector<CColor>> vv = vector<vector<CColor>>();
    for (int c = 0; c < col; ++c) {
        vv.push_back(vector<CColor>());
    }

    int hit_num = 0;

    for (int c = 0; c < col; ++c) {
        for (int r = 0; r < row; ++r) {
            Ray *ray = new Ray(DPoint3(row / 2, col / 2, 0), DVector3(r - row / 2, c - col / 2, col / 2));

            double t = 0.0;
            if (sphere->Hit(*ray, t)) {
                vv[c].push_back(sphere->GetColor());
                ++hit_num;
            }
            else {
                vv[c].push_back(CColor(255, 0, 0));
            }
        }
    }

    Utilities::Tools::SaveImage(vv);


    cout << "hit_num = " << hit_num << " , not_hit_num = " << col * row - hit_num << " , sum_num = " << col * row << endl;


//#ifdef DEBUG
    cout << "Pass Enter:\n";
    int pass;
    cin >> pass;
//#endif
    return 0;
}