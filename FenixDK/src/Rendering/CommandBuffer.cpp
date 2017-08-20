#include "CommandBuffer.h"

namespace fdk
{
  namespace Rendering 
  {

    CommandBuffer::CommandBuffer()
    {

    }
    
    CommandBuffer* CommandBuffer::create(Memory::MemAllocator& rAllocator)
    {
      return rAllocator.create<CommandBuffer>();
    }
  }
}

