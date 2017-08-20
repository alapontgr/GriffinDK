#pragma once
#include "Utilities\types.h"
#include "CommandBuffer.h"

#include <vector>
#include "Memory\MemAllocator.h"

namespace fdk
{
  namespace Rendering 
  {
    // TODO: Adapt it for multi threading
    class CommandBufferFactory 
    {
    
    public:

      void init(const u32 bufferCount); // N-Buffering for N frames

      CommandBuffer* get_primary_command_buffer(Memory::MemAllocator& rAllocator);

      CommandBuffer* get_secondary_command_buffer(Memory::MemAllocator& rAllocator);

      void flip();

      void sync_command_buffers();

    private:

      void init_cache(const u32 index);

      using CmdBufferCache = std::vector<CommandBuffer*>;
      using CmdBufferCursor = std::vector<CommandBuffer*>::iterator;

      struct FrameGetter 
      {
        CmdBufferCache m_primaryCmdBuffers;
        CmdBufferCursor m_primaryCursor;
        CmdBufferCache m_secondaryCmdBuffers;
        CmdBufferCursor m_secondaryCursor;
      };

      std::vector<FrameGetter> m_frameCaches;
      u32 m_BufferingCount;
      u32 m_currentIndex;
    };

  }
}