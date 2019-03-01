#pragma once

#include "../../ForwardDeclaration.h"
#include <thread>

namespace common
{
namespace tool
{

// TODO
// Ïß³Ì³Ø

extern thread_local int ThreadIndex;

int NumSystemCores();

int MaxThreadIndex();


void ParallelFor(std::function<void(int64_t)> func, int64_t count, int chunkSize = 1);

void ParallelFor2D(std::function<void(common::math::Vec2i)> func, const common::math::Vec2i &count);


}
}