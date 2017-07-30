#include "CommandBuffer.h"
#include "RenderDefines.h"

#include "VK_CommandBuffer.h"

namespace fdk
{
  namespace Rendering 
  {

    CommandBuffer::CommandBuffer()
    {

    }
    
    CommandBuffer* CommandBuffer::create(Memory::MemAllocator& rAllocator)
    {
      return rAllocator.create<IMPL_NAME(CommandBuffer)>();
    }
  }
}

