#pragma once

#include "Memory\MemAllocator.h"

namespace fdk
{
  namespace Rendering 
  {
    class CommandBuffer 
    {
    public:

      static CommandBuffer* create(Memory::MemAllocator& rAllocator);

    protected:
      
      CommandBuffer();
    };

  }
}