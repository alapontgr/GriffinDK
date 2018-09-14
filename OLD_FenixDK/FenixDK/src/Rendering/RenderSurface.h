#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk
{
  namespace Rendering 
  {
    class Texture2D;

    class RenderSurface 
    {
    public:

      RenderSurface() {}

      u32 m_width;
      u32 m_height;
      Texture2D* m_pColorTexture;
    };
  }
}