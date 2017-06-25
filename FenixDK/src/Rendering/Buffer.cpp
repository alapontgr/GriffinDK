#include "Buffer.h"

namespace fdk
{
  namespace Rendering 
  {

    void Buffer::init(
      const UsageFlags& bufferUsage,
      const MemoryProperties& memoryProperties,
      Memory::MemAllocator& rAllocator, 
      Memory::mem_size_t size, 
      Memory::mem_size_t alignment, 
      Memory::mem_ptr_t pInitialData)
    {
      FDK_ASSERT(this->size() == 0, "A buffer must be freed before allocating a new chunk of memory");
      m_data.allocate(rAllocator, size, alignment);
      if (pInitialData) 
      {
        memcpy(m_data.memory(), pInitialData, size);
      }
      m_usage = bufferUsage;
      m_memProperties = memoryProperties;
    }

    void Buffer::release(Memory::MemAllocator& rAllocator)
    {
      if (this->size() > 0) 
      {
        m_data.release(rAllocator);
      }
    }

    Memory::mem_ptr_t Buffer::data_mutable()
    {
      return m_data.memory();
    }

    Buffer::Buffer() {}

  }
}

