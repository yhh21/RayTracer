#pragma once

#include "pch.h"
#include <vector>
#include "Object.h"

namespace Objects
{
    class ObjectsManager
    {
    public:
        void AddObject(Object* obj)
        {
            objs.push_back(obj);
        }

        vector<Object*> GetObjects() const
        {
            return objs;
        }
    private:
        vector<Object*> objs;
    };
}