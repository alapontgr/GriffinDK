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

      void start_recording();

      void end_recording();

      VkCommandBuffer m_commandBuffer;
    };

  }
}