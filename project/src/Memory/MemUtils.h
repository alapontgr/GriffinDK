#pragma once
#include "Utilities\platform.h"
#include "Utilities\types.h"

#include <malloc.h>
#include <stdlib.h>

#if defined(WinPlatform)
#include <malloc.h>
#include <cstdlib>
#elif defined(UnixPlatform)
#include <stdlib.h>
#endif

namespace fdk
{
namespace Memory
{

	template <typename MemType>
	MemType* allocate_memory(u32 size, u32 alignment)
	{
#if defined(WinPlatform)
    return reinterpret_cast<MemType*>(_aligned_malloc(size, alignment));
#elif defined(UnixPlatform)
    return reinterpret_cast<MemType*>(memalign(alignment, size));
#endif
  }

  void release_memory(char* memBlock) 
  {
#if defined(WinPlatform)
    _aligned_free(memBlock);
#elif defined(UnixPlatform)
    free(memBlock);
#endif
  }
}
}