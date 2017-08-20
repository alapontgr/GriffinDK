#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk
{
  namespace Rendering 
  {
    class VK_Buffer
    {
    public:
      VK_Buffer();

      VkBuffer m_pBuffer;
      VkDeviceMemory m_pMemory;
    };
  }
}