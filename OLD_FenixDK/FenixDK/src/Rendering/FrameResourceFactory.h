#pragma once
#include "Utilities\types.h"
#include "Memory\MemAllocator.h"

#include "CommandBuffer.h"
#include "Framebuffer.h"
#include "Fence.h"

#include <vector>
#include <unordered_map>

namespace fdk
{
  namespace Rendering 
  {
    class RenderPass;

    // TODO: Adapt it for multi threading
    class FrameResourceFactory 
    {    
    public:

      void init(const u32 bufferCount); // N-Buffering for N frames

      void create_render_pass_resources(const RenderPass* pRenderPass);

      CommandBuffer* get_primary_command_buffer(Memory::MemAllocator& rAllocator);

      CommandBuffer* get_secondary_command_buffer(Memory::MemAllocator& rAllocator);

      void flip();

      Fence* get_current_fence();

      Framebuffer* get_render_pass_framebuffer(const RenderPass* pRenderPass);

    private:

      void init_cache(const u32 index);

      using CmdBufferCache = std::vector<CommandBuffer*>;
      using CmdBufferCursor = std::vector<CommandBuffer*>::iterator;
      using FramebufferCache = std::unordered_map<const RenderPass*, Framebuffer>;

      struct FrameResources 
      {
        CmdBufferCache m_primaryCmdBuffers;
        CmdBufferCache m_secondaryCmdBuffers;
        CmdBufferCursor m_primaryCmdCursor;
        CmdBufferCursor m_secondaryCmdCursor;
        FramebufferCache m_framebufferCache;
        Fence m_cmdBufferFence;
      };

      std::vector<FrameResources> m_frameCaches;
      u32 m_BufferingCount;
      u32 m_currentIndex;
    };

  }
}