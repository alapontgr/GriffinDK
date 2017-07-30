#pragma once

#include "Utilities\types.h"
#include "Containers\Mask.h"
#include "Memory\MemAllocator.h"

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
  class CommandBuffer;

	class RenderInterface
	{
	public:
		enum ERenderFlags : u32
		{
			kPendingResize = 1 << 0
		};

		static RenderInterface* create();

		void init();

		void release();

		bool is_pending_resize() const { m_flags.is_enable(kPendingResize); };

		void on_resize();

		void create_buffer(Buffer& rBuffer);

		void destroy_buffer(Buffer& rBuffer);

		void copy_buffer(Buffer& rFrom, Buffer& rTo);

		void send_buffer_memory_to_gpu(Buffer& rBuffer);

		void create_mesh(Framework::Mesh& rMesh, Memory::MemAllocator& rAllocator);

		void use_mesh(Framework::Mesh& rMesh);

		void bind_material(Material& rMaterial);

		void set_viewport(const Viewport& rViewport);

		void set_scissor(const Scissor& rScissor);

		void draw_indexed(const u32 indexCount, const u32 instanceCount, const u32 indexOffset, const u32 vertexOffset);

    void create_command_buffer(CommandBuffer& rCommandBuffer);

    void* map_buffer_gpu_memory(Buffer& rBuffer, const u32 memoryOffset, const u32 rangeSize);

    void unmap_buffer_gpu_memory(Buffer& rBuffer);

	protected:
		RenderInterface();

		Containers::Mask<u32> m_flags;
	};
}
}