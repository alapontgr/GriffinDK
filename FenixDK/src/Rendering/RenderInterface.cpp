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

	void RenderInterface::copy_buffer(Buffer& rFrom, const u32 fromOffset, Buffer& rTo, const u32 toOffset, const u32 rangeSize, CommandBuffer& rCmdBuffer)
	{
		IMPLEMENTATION(RenderInterface, this)->copy_buffer(rFrom, fromOffset, rTo, toOffset, rangeSize, rCmdBuffer);
	}

	void RenderInterface::send_buffer_memory_to_gpu(Buffer& rBuffer)
	{
		IMPLEMENTATION(RenderInterface, this)->send_buffer_memory_to_gpu(rBuffer);
	}

	void RenderInterface::use_mesh(Framework::Mesh& rMesh, CommandBuffer& rCmdBuffer)
	{
		IMPLEMENTATION(RenderInterface, this)->use_mesh(rMesh, rCmdBuffer);
	}

	void RenderInterface::bind_material(Material& rMaterial, CommandBuffer& rCmdBuffer)
	{
		IMPLEMENTATION(RenderInterface, this)->bind_material(rMaterial, rCmdBuffer);
	}

	void RenderInterface::set_viewport(const Viewport& rViewport, CommandBuffer& rCmdBuffer)
	{
		IMPLEMENTATION(RenderInterface, this)->set_viewport(rViewport, rCmdBuffer);
	}

	void RenderInterface::set_scissor(const Scissor& rScissor, CommandBuffer& rCmdBuffer)
	{
		IMPLEMENTATION(RenderInterface, this)->set_scissor(rScissor, rCmdBuffer);
	}

	void RenderInterface::draw_indexed(const u32 indexCount, const u32 instanceCount, const u32 indexOffset, const u32 vertexOffset, CommandBuffer& rCmdBuffer)
	{
		IMPLEMENTATION(RenderInterface, this)->draw_indexed(indexCount, instanceCount, indexOffset, vertexOffset, rCmdBuffer);
	}

  CommandBuffer* RenderInterface::get_command_buffer(CommandBuffer::ECommandBufferType type, Memory::MemAllocator& rAllocator)
  {
    return IMPLEMENTATION(RenderInterface, this)->get_command_buffer(type, rAllocator);
  }

  void RenderInterface::create_command_buffer(CommandBuffer& rCommandBuffer)
	{
		IMPLEMENTATION(RenderInterface, this)->create_command_buffer(rCommandBuffer);
	}

  void RenderInterface::wait_command_buffer_to_finish(CommandBuffer& rCmdBuffer)
  {
    IMPLEMENTATION(RenderInterface, this)->wait_command_buffer_to_finish(rCmdBuffer);
  }

  void* RenderInterface::map_buffer_gpu_memory(Buffer& rBuffer, const u32 memoryOffset, const u32 rangeSize)
  {
    return IMPLEMENTATION(RenderInterface, this)->map_buffer_gpu_memory(rBuffer, memoryOffset, rangeSize);
  }

  void RenderInterface::unmap_buffer_gpu_memory(Buffer& rBuffer)
  {
    IMPLEMENTATION(RenderInterface, this)->unmap_buffer_gpu_memory(rBuffer);
  }

  void RenderInterface::beginFrame()
  {
    IMPLEMENTATION(RenderInterface, this)->beginFrame();
  }

  void RenderInterface::endFrame()
  {
    IMPLEMENTATION(RenderInterface, this)->endFrame();
  }

}
}
