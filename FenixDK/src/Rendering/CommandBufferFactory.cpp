#include "CommandBufferFactory.h"
#include "Maths\math_utils.h"
#include "RenderInterface.h"

namespace fdk
{
  namespace Rendering 
  {
    
    void CommandBufferFactory::init(const u32 bufferCount)
    {
      m_currentIndex = 0;
      m_BufferingCount = bufferCount;
      m_frameCaches.resize(bufferCount);
      for (u32 i=0; i<bufferCount; i++) 
      {
        init_cache(i);
      }
    }

    CommandBuffer* CommandBufferFactory::get_primary_command_buffer(RenderInterface& rRi, Memory::MemAllocator& rAllocator)
    {
      auto& rCache = m_frameCaches[m_currentIndex];
      if (rCache.m_primaryCursor != rCache.m_primaryCmdBuffers.end()) 
      {
        auto* pCmdBuffer = *rCache.m_primaryCursor;
        rCache.m_primaryCursor++;
        return pCmdBuffer;
      }
      // Else create a new one
      auto* pCmdBuffer = CommandBuffer::create(rAllocator);
      pCmdBuffer->set_type(CommandBuffer::kTypePrimary);
      rRi.create_command_buffer(*pCmdBuffer);

      rCache.m_primaryCmdBuffers.push_back(pCmdBuffer);
      rCache.m_primaryCursor = rCache.m_primaryCmdBuffers.end();
      return pCmdBuffer;
    }

    CommandBuffer* CommandBufferFactory::get_secondary_command_buffer(RenderInterface& rRi, Memory::MemAllocator& rAllocator)
    {
      auto& rCache = m_frameCaches[m_currentIndex];
      if (rCache.m_secondaryCursor != rCache.m_secondaryCmdBuffers.end())
      {
        auto* pCmdBuffer = *rCache.m_secondaryCursor;
        rCache.m_secondaryCursor++;
        return pCmdBuffer;
      }
      // Else create a new one
      auto* pCmdBuffer = CommandBuffer::create(rAllocator);
      pCmdBuffer->set_type(CommandBuffer::kTypeSecondary);
      rRi.create_command_buffer(*pCmdBuffer);

      rCache.m_secondaryCmdBuffers.push_back(pCmdBuffer);
      rCache.m_secondaryCursor = rCache.m_secondaryCmdBuffers.end();
      return pCmdBuffer;
    }

    void CommandBufferFactory::flip()
    {
      m_currentIndex = Maths::wrap(m_currentIndex + 1, 0U, m_BufferingCount);

      auto& rCache = m_frameCaches[m_currentIndex];
      rCache.m_primaryCursor = rCache.m_primaryCmdBuffers.begin();
      rCache.m_secondaryCursor = rCache.m_secondaryCmdBuffers.begin();
    }

    void CommandBufferFactory::sync_command_buffers()
    {
      // TODO: Wait for the fence of the frame to be signaled
    }

    void CommandBufferFactory::init_cache(const u32 index)
    {
      FrameGetter& rCache = m_frameCaches[index];
      rCache.m_primaryCursor = rCache.m_primaryCmdBuffers.begin();
      rCache.m_secondaryCursor = rCache.m_secondaryCmdBuffers.begin();
    }
  }
}

