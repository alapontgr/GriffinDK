#pragma once

#include "Utilities\types.h"
#include "Containers\Mask.h"
#include "Memory\MemAllocator.h"

#include "CommandBuffer.h"
#include "CommandBufferFactory.h"

namespace fdk
{
namespace Framework
{
	class Mesh;
}

namespace Rendering
{
	class Buffer;
	class Material;
	struct Viewport;
	struct Scissor;

	class RenderInterface
	{
	public:

		static RenderInterface* create();

		void init();

		void release();

		void on_resize();

		void create_buffer(Buffer& rBuffer);

		void destroy_buffer(Buffer& rBuffer);

		void copy_buffer(Buffer& rFrom, const u32 fromOffset, Buffer& rTo, const u32 toOffset, const u32 rangeSize, CommandBuffer& rCmdBuffer);

		void send_buffer_memory_to_gpu(Buffer& rBuffer);

		void use_mesh(Framework::Mesh& rMesh, CommandBuffer& rCmdBuffer);

		void bind_material(Material& rMaterial, CommandBuffer& rCmdBuffer);

		void set_viewport(const Viewport& rViewport, CommandBuffer& rCmdBuffer);

		void set_scissor(const Scissor& rScissor, CommandBuffer& rCmdBuffer);

		void draw_indexed(const u32 indexCount, const u32 instanceCount, const u32 indexOffset, const u32 vertexOffset, CommandBuffer& rCmdBuffer);

    void create_command_buffer(CommandBuffer& rCommandBuffer);

    void* map_buffer_gpu_memory(Buffer& rBuffer, const u32 memoryOffset, const u32 rangeSize);

    void unmap_buffer_gpu_memory(Buffer& rBuffer);

    void beginFrame();

    void endFrame();

  protected: 
    
    RenderInterface();
  };
}
}