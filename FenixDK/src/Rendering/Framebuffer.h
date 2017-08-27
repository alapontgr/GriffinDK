#pragma once

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "VK_Framebuffer.h"
#endif

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
        const RenderInterface& rRI,
        const RenderPass& rRenderPass,
        const RenderSurface& rRenderSurface);

      void destroy(const RenderInterface& rRI);
    };

  }
}