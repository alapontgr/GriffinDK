#pragma once
#include "MemAllocator.h"
#include "MemUtils.h"

namespace fdk
{
  namespace Memory 
  {
    class MallocAllocator : public MemAllocator
    {
    public:
      virtual ~MallocAllocator() {}

      virtual mem_ptr_t allocate(const mem_size_t size, const mem_align_t alignment) 
      {
        return allocate_memory(size, alignment);
      }

      virtual void release(mem_ptr_t pMemory) 
      {
        FDK_ASSERT(pMemory, "Invalid memory block");
        release_memory(pMemory);
      }
    };
  }
}