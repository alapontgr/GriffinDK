#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk
{
  namespace Rendering 
  {
    class RenderInterface;

    class VK_Fence 
    {
    public:

      VK_Fence();

      void wait(const RenderInterface& rRi);

      VkFence m_fence;
    };

  }
}