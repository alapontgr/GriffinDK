#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk
{
  namespace Rendering 
  {
    class RenderSurface 
    {
    public:

      u32 m_width;
      u32 m_height;
      VkImageView m_colorTexture;
    };
  }
}