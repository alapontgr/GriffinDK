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

    void RenderInterface::create_buffer(Buffer& rBuffer)
    {
      IMPLEMENTATION(RenderInterface, this)->create_buffer(rBuffer);
    }

    void RenderInterface::destroy_buffer(Buffer& rBuffer)
    {
      IMPLEMENTATION(RenderInterface, this)->destroy_buffer(rBuffer);
    }

    void RenderInterface::copy_buffer(Buffer& rFrom, Buffer& rTo)
    {
      IMPLEMENTATION(RenderInterface, this)->copy_buffer(rFrom, rTo);
    }

    void RenderInterface::send_buffer_memory_to_gpu(Buffer& rBuffer)
    {
      IMPLEMENTATION(RenderInterface, this)->send_buffer_memory_to_gpu(rBuffer);
    }
  }
}

