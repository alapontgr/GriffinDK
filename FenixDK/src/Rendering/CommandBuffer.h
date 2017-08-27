#pragma once

#include "Memory\MemAllocator.h"
#include "Containers\Mask.h"

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "VK_CommandBuffer.h"
#endif

namespace fdk
{
  namespace Rendering 
  {
    class CommandBuffer : public IMPLEMENTATION(CommandBuffer)
    {
    public:

      using BaseT = IMPLEMENTATION(CommandBuffer);

      enum ECommandBufferType : u32 
      {
        kTypePrimary = 1<<0,
        kTypeSecondary = 1<<1
      };

      CommandBuffer();

      static CommandBuffer* create(Memory::MemAllocator& rAllocator);

      void set_type(ECommandBufferType type) { m_type = type; }

      u32 type() const { return m_type; }

      void start_recording();

      void end_recording();

    protected:

      u32 m_type;
    };

  }
}