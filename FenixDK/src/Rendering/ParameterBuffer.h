#pragma once

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "Vulkan/VK_ParameterBuffer.h"
#endif


namespace fdk
{
  namespace Rendering 
  {
    class ParameterBuffer : public IMPLEMENTATION(ParameterBuffer)
    {
    public:

      using BaseT = IMPLEMENTATION(ParameterBuffer);

      ParameterBuffer();

      void create(
        const MaterialParameterSet& rParameterSetLayout,
        const ParameterBufferGroup& rAllocationGroup,
        Memory::MemAllocator& rAllocator);

      void release();
      
      void update();
      
      void bind();

      u32 slot() const { return m_bindingSlot; }

      void set_bindingSlot(const u32 slot) { m_bindingSlot = slot; }
    };

  }
}
