#pragma once

#include "MaterialParameterSet.h"

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "Vulkan/VK_ParameterBuffer.h"
#endif

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

      void create(RenderInterface& rRI);

      void update();

      void release();

      u32 slot() const { return m_bindingSlot; }

      void set_bindingSlot(const u32 slot) { m_bindingSlot = slot; }

    private:

      MaterialParameterSet* m_pParameterSetLayout;
      u32 m_bindingSlot;
    };

  }
}
