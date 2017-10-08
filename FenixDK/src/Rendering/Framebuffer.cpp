#include "Framebuffer.h"
#include "RenderSurface.h"

namespace fdk
{
  namespace Rendering 
  {
    
    Framebuffer::Framebuffer()
    {

    }

    void Framebuffer::create(const RenderPass& rRenderPass, const RenderSurface& rRenderSurface)
    {
      m_width = rRenderSurface.m_width;
      m_height = rRenderSurface.m_height;
      BaseT::create(rRenderPass, rRenderSurface);
    }

    void Framebuffer::destroy()
{
      BaseT::destroy();
    }

  }
}

