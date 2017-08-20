#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk
{
  namespace Rendering
  {
    class VK_CommandBuffer
    {
    public:

      VK_CommandBuffer();

      VkCommandBuffer m_commandBuffer;
    };

  }
}