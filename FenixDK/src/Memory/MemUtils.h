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
  using mem_t = char;
  using mem_ptr_t = mem_t*;
  using mem_size_t = size_t;
  using mem_align_t = size_t;

	inline mem_ptr_t allocate_memory(mem_size_t size, mem_align_t alignment)
	{
#if defined(WinPlatform)
    return reinterpret_cast<mem_ptr_t>(_aligned_malloc(size, alignment));
#elif defined(UnixPlatform)
    return reinterpret_cast<mem_ptr_t>(memalign(alignment, size));
#endif
  }

  inline void release_memory(char* memBlock)
  {
#if defined(WinPlatform)
    _aligned_free(memBlock);
#elif defined(UnixPlatform)
    free(memBlock);
#endif
  }

  inline bool is_aligned(const mem_ptr_t pData, const mem_align_t alignment)
  {
    return (reinterpret_cast<mem_size_t>(pData) & (alignment-1)) == 0;
  }

  inline mem_size_t get_alignment_offset(mem_ptr_t pData, const mem_align_t alignment)
  {
    auto offset = (reinterpret_cast<mem_size_t>(pData) & (alignment - 1));
    return offset != 0 ? (alignment - offset) : 0;
  }

  inline mem_size_t mem_distance(const mem_ptr_t pA, const mem_ptr_t pB)
  {
    return pB - pA;
  }
}
}