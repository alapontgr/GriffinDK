#pragma once

#include "CommandBuffer.h"

#include "Utilities/VulkanInclude.h"

namespace fdk
{
  namespace Rendering 
  {
    class VK_CommandBuffer : public CommandBuffer
    {
    public:

      VK_CommandBuffer();

      VkCommandBuffer m_commandBuffer;
    };

  }
}