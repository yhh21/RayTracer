#include "pch.h"

#include "Scene.hpp"

int main() {
#ifdef DEBUG
    Common::DebugTools::PrintDebugLog("hello, ray tracer!\n", false);
#endif


    Scene world;
    world.BuildWord();
    world.RenderScene();


//#ifdef DEBUG
    Common::DebugTools::PrintDebugLog("Pass Enter:\n", false);
    int pass;
    cin >> pass;
//#endif
    return 0;
}