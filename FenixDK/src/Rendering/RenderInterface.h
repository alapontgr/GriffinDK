#pragma once

#include "Utilities\types.h"
#include "Containers\Mask.h"
#include "Memory\MemAllocator.h"

#include "CommandBuffer.h"
#include "FrameResourceFactory.h"

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "VK_RenderInterface.h"
#endif

namespace fdk
{
namespace Rendering
{
	class RenderInterface : public IMPLEMENTATION(RenderInterface)
	{
	public:

    using BaseT = IMPLEMENTATION(RenderInterface);

		static RenderInterface* instance();

    RenderInterface();

		void init();

		void release();

		void on_resize();

    Texture2D* get_screen();

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

    void create_fence(Fence& rFence);

    void submit_work(const CommandBuffer& rCmdBuffer, const Fence& rSyncFence);

    void beginFrame();

    void endFrame();
  };
}
}