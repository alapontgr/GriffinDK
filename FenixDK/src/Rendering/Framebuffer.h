#pragma once

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "Vulkan/VK_Framebuffer.h"
#endif
#include "Utilities/types.h"

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

      void destroy(const RenderInterface& rRI);

      u32 width() const { return m_width; }
      u32 height() const { return m_height; }

    private:

      u32 m_width;
      u32 m_height;
    };

  }
}