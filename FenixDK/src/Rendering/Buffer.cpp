#include "Buffer.h"

#include "VK_Buffer.h"

#include "RenderDefines.h"

namespace fdk
{
  namespace Rendering 
  {

    void Buffer::init(
      const BufferDesc& rDesc,
      Memory::MemAllocator& rAllocator,
      Memory::mem_ptr_t pInitialData)
    {
      m_desc = rDesc;
      FDK_ASSERT(this->size() == 0, "A buffer must be freed before allocating a new chunk of memory");
      if (pInitialData) 
      {
        m_data.allocate(rAllocator, rDesc.m_size, rDesc.m_alignment);
        memcpy(m_data.memory(), pInitialData, rDesc.m_size);
      }
    }

    void Buffer::release(Memory::MemAllocator& rAllocator)
    {
      if (m_data.size() > 0) 
      {
        m_data.release(rAllocator);
      }
    }

    Memory::mem_ptr_t Buffer::data_mutable()
    {
      return m_data.memory();
    }

    Buffer::Buffer() {}


    Buffer* Buffer::create(Memory::MemAllocator& rAllocator)
    {
      return rAllocator.create<IMPL_NAME(Buffer)>();
    }

  }
}

