#include "RenderInterface.h"
#include "RenderDefines.h"

// Implementations
#include "VK_RenderInterface.h"

namespace fdk
{
  namespace Rendering 
  { 
    RenderInterface::RenderInterface() {} 

    void RenderInterface::init()
    {
      IMPLEMENTATION(RenderInterface, this)->init();
    }

    void RenderInterface::release()
    {
      IMPLEMENTATION(RenderInterface, this)->release();
    }

    void RenderInterface::on_resize()
    {
      IMPLEMENTATION(RenderInterface, this)->on_resize();
    }

  }
}

