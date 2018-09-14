#include "FrameResourceFactory.h"
#include "Maths\math_utils.h"
#include "RenderInterface.h"
#include "Framebuffer.h"

namespace fdk
{
  namespace Rendering 
  {
    
    void FrameResourceFactory::init(const u32 bufferCount)
    {
      m_currentIndex = 0;
      m_BufferingCount = bufferCount;
      m_frameCaches.resize(bufferCount);
      for (u32 i=0; i<bufferCount; i++) 
      {
        init_cache(i);
      }
    }

    void FrameResourceFactory::create_render_pass_resources(const RenderPass* pRenderPass)
    {
      for (auto& rFrame : m_frameCaches) 
      {
        auto it = rFrame.m_framebufferCache.find(pRenderPass);
        FDASSERT(it == rFrame.m_framebufferCache.end(), "This Render pass has already created its resources");
        rFrame.m_framebufferCache[pRenderPass] = Framebuffer();
      }
    }

    fdk::Rendering::CommandBuffer* FrameResourceFactory::get_primary_command_buffer(Memory::MemAllocator& rAllocator)
    {
      auto& rCache = m_frameCaches[m_currentIndex];
      if (rCache.m_primaryCmdCursor != rCache.m_primaryCmdBuffers.end()) 
      {
        auto* pCmdBuffer = *rCache.m_primaryCmdCursor;
        rCache.m_primaryCmdCursor++;
        return pCmdBuffer;
      }
      // Else create a new one
      auto* pCmdBuffer = CommandBuffer::create(rAllocator);
      pCmdBuffer->set_type(CommandBuffer::kTypePrimary);
      RenderInterface::create_command_buffer(*pCmdBuffer);

      rCache.m_primaryCmdBuffers.push_back(pCmdBuffer);
      rCache.m_primaryCmdCursor = rCache.m_primaryCmdBuffers.end();
      return pCmdBuffer;
    }

    fdk::Rendering::CommandBuffer* FrameResourceFactory::get_secondary_command_buffer(Memory::MemAllocator& rAllocator)
    {
      auto& rCache = m_frameCaches[m_currentIndex];
      if (rCache.m_secondaryCmdCursor != rCache.m_secondaryCmdBuffers.end())
      {
        auto* pCmdBuffer = *rCache.m_secondaryCmdCursor;
        rCache.m_secondaryCmdCursor++;
        return pCmdBuffer;
      }
      // Else create a new one
      auto* pCmdBuffer = CommandBuffer::create(rAllocator);
      pCmdBuffer->set_type(CommandBuffer::kTypeSecondary);
      RenderInterface::create_command_buffer(*pCmdBuffer);

      rCache.m_secondaryCmdBuffers.push_back(pCmdBuffer);
      rCache.m_secondaryCmdCursor = rCache.m_secondaryCmdBuffers.end();
      return pCmdBuffer;
    }

    void FrameResourceFactory::flip()
    {
      m_currentIndex = Maths::wrap(m_currentIndex + 1, 0U, m_BufferingCount);

      auto& rCache = m_frameCaches[m_currentIndex];
      rCache.m_primaryCmdCursor = rCache.m_primaryCmdBuffers.begin();
      rCache.m_secondaryCmdCursor = rCache.m_secondaryCmdBuffers.begin();
    }

    Fence* FrameResourceFactory::get_current_fence()
    {
      FrameResources& rCache = m_frameCaches[m_currentIndex];
      return &rCache.m_cmdBufferFence;
    }

    Framebuffer* FrameResourceFactory::get_render_pass_framebuffer(const RenderPass* pRenderPass)
    {
      FrameResources& rCache = m_frameCaches[m_currentIndex];
      return &rCache.m_framebufferCache[pRenderPass];
    }

    void FrameResourceFactory::init_cache(const u32 index)
    {
      FrameResources& rCache = m_frameCaches[index];
      rCache.m_primaryCmdCursor = rCache.m_primaryCmdBuffers.begin();
      rCache.m_secondaryCmdCursor = rCache.m_secondaryCmdBuffers.begin();
      RenderInterface::create_fence(rCache.m_cmdBufferFence);
    }
  }
}

