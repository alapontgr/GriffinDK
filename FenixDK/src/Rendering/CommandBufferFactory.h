#pragma once
#include "Utilities\types.h"
#include "Memory\MemAllocator.h"

#include "CommandBuffer.h"
#include "Fence.h"

#include <vector>

namespace fdk
{
  namespace Rendering 
  {
    class RenderInterface;

    // TODO: Adapt it for multi threading
    class CommandBufferFactory 
    {    
    public:

      void init(const u32 bufferCount, RenderInterface& rRi); // N-Buffering for N frames

      CommandBuffer* get_primary_command_buffer(RenderInterface& rRi, Memory::MemAllocator& rAllocator);

      CommandBuffer* get_secondary_command_buffer(RenderInterface& rRi, Memory::MemAllocator& rAllocator);

      void flip();

      Fence* get_current_fence();

    private:

      void init_cache(const u32 index, RenderInterface& rRi);

      using CmdBufferCache = std::vector<CommandBuffer*>;
      using CmdBufferCursor = std::vector<CommandBuffer*>::iterator;

      struct FrameGetter 
      {
        CmdBufferCache m_primaryCmdBuffers;
        CmdBufferCursor m_primaryCursor;
        CmdBufferCache m_secondaryCmdBuffers;
        CmdBufferCursor m_secondaryCursor;
        Fence m_cmdBufferFence;
      };

      std::vector<FrameGetter> m_frameCaches;
      u32 m_BufferingCount;
      u32 m_currentIndex;
    };

  }
}