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

    void RenderInterface::use_mesh(Framework::Mesh& rMesh)
    {
      IMPLEMENTATION(RenderInterface, this)->use_mesh(rMesh);
    }

    void RenderInterface::bind_material(Material& rMaterial)
    {
      IMPLEMENTATION(RenderInterface, this)->bind_material(rMaterial);
    }

    void RenderInterface::set_viewport(const Viewport& rViewport)
    {
      IMPLEMENTATION(RenderInterface, this)->set_viewport(rViewport);
    }

    void RenderInterface::set_scissor(const Scissor& rScissor)
    {
      IMPLEMENTATION(RenderInterface, this)->set_scissor(rScissor);
    }

    void RenderInterface::draw_indexed(const u32 indexCount, const u32 instanceCount, const u32 indexOffset, const u32 vertexOffset)
    {
      IMPLEMENTATION(RenderInterface, this)->draw_indexed(indexCount, instanceCount, indexOffset, vertexOffset);
    }

  }
}

