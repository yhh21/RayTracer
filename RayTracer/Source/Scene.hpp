#pragma once

#include "pch.h"
#include "ObjectsManager.hpp"
#include "Color.hpp"
#include "Ray.hpp"
#include "Sphere.hpp"
#include "Window.hpp"
#include "ImageManager.hpp"

namespace Utilities {
    class Scene {
    public:
        void BuildWord() {
            size_t col = 300, row = 300;
            window = new Utilities::Window(col, row);

            Objects::Sphere *sphere1 = new Objects::Sphere(CColor_WHITE, DPoint3(row / 2, col / 2 - col / 8, col / 2), col / 4);
            Objects::Sphere *sphere2 = new Objects::Sphere(CColor_GREEN, DPoint3(row / 2, col / 2 + col / 8, col / 2 + col / 8), col / 4);

            objsMgr.AddObject(sphere1);
            objsMgr.AddObject(sphere2);
        }

        void RenderScene() {
            int col = window->GetCol(), row = window->GetRow();

            vector<vector<CColor>> vv = vector<vector<CColor>>();
            for (int c = 0; c < col; ++c) {
                vv.push_back(vector<CColor>());
            }
#ifdef DEBUG
            size_t hit_num = 0;
#endif // DEBUG
            CColor *pColor = nullptr;
            auto objs = objsMgr.GetObjects();
            size_t objsLength = objs.size();
            DPoint3 eye(row / 2, col / 2, 0);

            for (int c = 0; c < col; ++c) {
                for (int r = 0; r < row; ++r) {
                    Ray *ray = new Ray(eye, DVector3(r - row / 2, c - col / 2, col / 2));

                    bool isHit = false;
                    double tMin;
                    double t;
                    for (size_t i = 0; i < objsLength; ++i) {
                        if (objs[i]->Hit(*ray, t) && (!isHit || t < tMin)) {
                            pColor = &(objs[i]->GetColor());
                            isHit = true;
                            tMin = t;
#ifdef DEBUG
                                ++hit_num;
#endif //DEBUG
                        }
                    }
                    
                    vv[c].push_back(isHit ? *pColor : CColor_RED);
                    //if (isHit) pColor->Print();
                }
            }

            Utilities::ImageManager::SaveImage(vv);


#ifdef DEBUG
            string str ="hit_num = " + to_string(hit_num) + " , not_hit_num = " + to_string(col * row - hit_num) + " , sum_num = " + to_string(col * row) + "\n";
            Utilities::DebugTools::PrintDebugLog(str);
#endif // DEBUG
        }


    private:
        Objects::ObjectsManager objsMgr;
        Utilities::Window *window;
    };
}