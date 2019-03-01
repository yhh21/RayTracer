#include "MemoryArena.h"

namespace common
{
namespace tool
{


void *AllocAligned(size_t size)
{
    const size_t L1_CACHE_LINE_SIZE = 64;
    return _aligned_malloc(size, L1_CACHE_LINE_SIZE);
}

void FreeAligned(void *ptr)
{
    if (nullptr != ptr)
    {
        _aligned_free(ptr);
    }
}


}
}