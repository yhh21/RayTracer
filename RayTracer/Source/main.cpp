#include "pch.h"

#include "Scene.hpp"

#include <iostream>
using namespace std;


int main() {
    cout << "hello, ray tracer!\n";


    Scene world;
    world.BuildWord();
    world.RenderScene();


//#ifdef DEBUG
    cout << "Pass Enter:\n";
    int pass;
    cin >> pass;
//#endif
    return 0;
}