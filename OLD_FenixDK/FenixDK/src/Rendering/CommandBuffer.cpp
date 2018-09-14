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

    void CommandBuffer::start_recording()
    {
      BaseT::start_recording();
    }

    void CommandBuffer::end_recording()
    {
      BaseT::end_recording();
    }

  }
}

