#pragma once

#include "Utilities/types.h"

#include "Utilities/platform.h"
#include INCLUDE_IMPLEMENTATION(Framebuffer)

namespace fdk
{
  namespace Rendering 
  {
    class Framebuffer : public IMPLEMENTATION(Framebuffer)
    {
    public:

      using BaseT = IMPLEMENTATION(Framebuffer);

      Framebuffer();

      void create(
        const RenderPass& rRenderPass,
        const RenderSurface& rRenderSurface);

      void destroy();

      u32 width() const { return m_width; }
      u32 height() const { return m_height; }

    private:

      u32 m_width;
      u32 m_height;
    };

  }
}