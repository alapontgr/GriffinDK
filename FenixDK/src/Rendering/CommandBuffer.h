#pragma once

#include "Memory\MemAllocator.h"
#include "Containers\Mask.h"

namespace fdk
{
  namespace Rendering 
  {
    class CommandBuffer 
    {
    public:

      enum ECommandBufferType : u32 
      {
        kTypePrimary = 1<<0,
        kTypeSecondary = 1<<1
      };

      static CommandBuffer* create(Memory::MemAllocator& rAllocator);

      void set_type(ECommandBufferType type) { m_type = type; }

      u32 type() const { return m_type; }

    protected:
      
      CommandBuffer();

      u32 m_type;
    };

  }
}