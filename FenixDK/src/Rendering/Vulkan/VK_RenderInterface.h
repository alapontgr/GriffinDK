#pragma once

#include "Utilities/types.h"
#include <vector>
#include "Containers/Mask.h"

#include "Utilities/VulkanInclude.h"

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
	class Texture2D;
	class CommandBuffer;
  class ParameterBuffer;
	struct Viewport;
	struct Scissor;
	class Fence;

	namespace Vulkan
	{
		struct FrameInfoVK
		{
			//VkFramebuffer frameBuffer = VK_NULL_HANDLE;
			VkSemaphore imageAvailable = VK_NULL_HANDLE; // Signaled by the presentation engine with the image is available
			VkSemaphore finishedRendering = VK_NULL_HANDLE; // Signaled when a queue has finished being processed
		};

		class VK_RenderInterface
		{
		public:

			VK_RenderInterface();
			~VK_RenderInterface();

      static void init();

      static void release();

      static void on_resize();

      static Texture2D* get_screen();

      static void create_buffer(Buffer& rBuffer);

      static void destroy_buffer(Buffer& rBuffer);

      static void use_mesh(Framework::Mesh& rMesh, CommandBuffer& rCmdBuffer);

      static void bind_material(Material& rMaterial, CommandBuffer& rCmdBuffer);

      static void bind_parameter_buffer(const Material& rMaterial, const ParameterBuffer& rParamBuffer, CommandBuffer& rCmdBuffer);

      static void set_viewport(const Viewport& rViewport, CommandBuffer& rCmdBuffer);

      static void set_scissor(const Scissor& rScissor, CommandBuffer& rCmdBuffer);

      static void draw_indexed(const u32 indexCount, const u32 instanceCount, const u32 indexOffset, const u32 vertexOffset, CommandBuffer& rCmdBuffer);

      static void create_command_buffer(CommandBuffer& rCommandBuffer);

      static void* map_buffer_gpu_memory(Buffer& rBuffer, const u32 memoryOffset, const u32 rangeSize);

      static void unmap_buffer_gpu_memory(Buffer& rBuffer);

      static void create_fence(Fence& rFence);

      static void beginFrame();

      static void endFrame();

      static void submit_work(const CommandBuffer& rCmdBuffer, const Fence& rsyncFence);

			//private:
      static void validate_vk_extensions();

      static void check_device_extensions(VkPhysicalDevice pPhysicalDevice);

      static bool check_physical_device_properties(VkPhysicalDevice device,
																						u32& selectedFamilyIndex,
																						u32& swapChainFamilyIndex);

      static void create_render_instance();

      static void create_screen_surface();

      static void create_logical_device();

      static void create_swap_chain();

      static void get_swap_chain_images();

      static void create_cmd_pool();

      static void clean_frame_infos();

      static void create_frame_infos();

      static void release_swap_chain();

      static void release_device();

      static void release_surface();

      static void release_render_instance();

			//Windows
			static VkSurfaceKHR s_surface;
			static VkSwapchainKHR s_swapChain;
			static VkSurfaceFormatKHR s_swapChainFormat;
			static VkSurfaceCapabilitiesKHR s_Capabilities;
			static std::vector<VkSurfaceFormatKHR> s_supportedFormats;
			static std::vector<VkPresentModeKHR> s_supportedPresentModes;
			//

			// Context state
      static VkInstance s_instance;
      static VkPhysicalDevice s_physicalDevice;
      static VkDevice s_device;
      static VkQueue s_graphicsQueue;
      static VkQueue s_presentQueue;
      static u32 s_graphicsFamilyIndex;
      static u32 s_presentFamilyIndex;
      static std::vector<VkImage> s_swapChainImages;
      static std::vector<VkImageView> s_swapChainImageView;
      static VkCommandPool s_commandPool;
      static std::vector<FrameInfoVK> s_frames; // We use N buffering so every frame we will need to use the correct frame info

			static u32 s_currentFrame;
			static u32 s_currentImageIndex;
		};
	}
}
}