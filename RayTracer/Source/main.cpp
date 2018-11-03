#include "pch.h"

#include "Scene.hpp"

int main() {
#ifdef DEBUG
    Utilities::DebugTools::PrintDebugLog("hello, ray tracer!\n", false);
#endif


    Scene world;
    world.BuildWord();
    world.RenderScene();


//#ifdef DEBUG
    Utilities::DebugTools::PrintDebugLog("Pass Enter:\n", false);
    int pass;
    cin >> pass;
//#endif
    return 0;
}