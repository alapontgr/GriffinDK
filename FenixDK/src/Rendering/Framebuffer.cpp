#include "Framebuffer.h"

namespace fdk
{
  namespace Rendering 
  {
    
    Framebuffer::Framebuffer()
    {

    }

    void Framebuffer::create(const RenderInterface& rRI, const RenderPass& rRenderPass, const RenderSurface& rRenderSurface)
    {
      BaseT::create(rRI, rRenderPass, rRenderSurface);
    }

    void Framebuffer::destroy(const RenderInterface& rRI)
    {
      BaseT::destroy(rRI);
    }

  }
}

