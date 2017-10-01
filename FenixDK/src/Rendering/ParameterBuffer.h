#pragma once

#include "MaterialParameterSet.h"

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "Vulkan/VK_ParameterBuffer.h"
#endif
#include "Memory/MemAllocator.h"

namespace fdk
{
  namespace Rendering 
  {
    class RenderInterface;

    class ParameterBuffer : public IMPLEMENTATION(ParameterBuffer)
    {
    public:

      using BaseT = IMPLEMENTATION(ParameterBuffer);

      ParameterBuffer();

      void create(RenderInterface& rRI, Memory::MemAllocator& rAllocator);

      void release(RenderInterface& rRI);
      
      void update();
      
      void bind();

      u32 slot() const { return m_bindingSlot; }

      void set_bindingSlot(const u32 slot) { m_bindingSlot = slot; }

    private:

      Memory::mem_ptr_t m_pParametersData;
      MaterialParameterSet* m_pParameterSetLayout;
      u32 m_bindingSlot;
    };

  }
}
