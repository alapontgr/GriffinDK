#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk
{
  namespace Rendering 
  {
    class VK_Fence 
    {
    public:

      VK_Fence() : m_fence{ VK_NULL_HANDLE } {}

      VkFence m_fence;
    };
  }
}