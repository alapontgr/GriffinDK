#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk
{
  namespace Rendering 
  {
    class VK_Texture2D 
    {
    public:

      VK_Texture2D();
      
      VkImageView m_view;
    };

  }
}