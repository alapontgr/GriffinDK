#pragma once

#include "Utilities\types.h"
#include "Containers\Mask.h"
#include "Memory\MemAllocator.h"

#include "CommandBuffer.h"
#include "FrameResourceFactory.h"

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "Vulkan/VK_RenderInterface.h"
#endif

namespace fdk
{
namespace Rendering
{
	class RenderInterface : public IMPLEMENTATION(RenderInterface)
	{
	public:

    using BaseT = IMPLEMENTATION(RenderInterface);

		static void init();

    static void release();

    static void on_resize();

    static Texture2D* get_screen();

    static void create_buffer(Buffer& rBuffer);

    static void destroy_buffer(Buffer& rBuffer);

    static void use_mesh(Framework::Mesh& rMesh, CommandBuffer& rCmdBuffer);

    static void bind_material(Material& rMaterial, CommandBuffer& rCmdBuffer);

    static void set_viewport(const Viewport& rViewport, CommandBuffer& rCmdBuffer);

    static void set_scissor(const Scissor& rScissor, CommandBuffer& rCmdBuffer);

    static void draw_indexed(const u32 indexCount, const u32 instanceCount, const u32 indexOffset, const u32 vertexOffset, CommandBuffer& rCmdBuffer);

    static void create_command_buffer(CommandBuffer& rCommandBuffer);

    static void create_fence(Fence& rFence);

    static void* map_buffer_gpu_memory(Buffer& rBuffer, const u32 memoryOffset, const u32 rangeSize);

    static void unmap_buffer_gpu_memory(Buffer& rBuffer);

    static void submit_work(const CommandBuffer& rCmdBuffer, const Fence& rSyncFence);

    static void beginFrame();

    static void endFrame();
  };
}
}