#pragma once

#include "RenderInterface.h"
#include "Utilities/types.h"
#include <vector>
#include "Containers/Mask.h"

#include "Utilities/VulkanInclude.h"

namespace fdk
{
namespace Rendering
{
  struct FrameInfoVK 
  {
    //VkFramebuffer frameBuffer = VK_NULL_HANDLE;
    VkSemaphore imageAvailable = VK_NULL_HANDLE; // Signaled by the presentation engine with the image is available
    VkSemaphore finishedRendering = VK_NULL_HANDLE; // Signaled when a queue has finished being processed   
  };

	class VK_RenderInterface : public RenderInterface
	{
	public:
		VK_RenderInterface();
		~VK_RenderInterface();

    void init();

    void release();

    void on_resize();

    void create_buffer(Buffer& rBuffer);

    void destroy_buffer(Buffer& rBuffer);

    void copy_buffer(Buffer& rFrom, Buffer& rTo);

    void send_buffer_memory_to_gpu(Buffer& rBuffer);

  private:

    void validate_vk_extensions();

    void check_device_extensions(VkPhysicalDevice pPhysicalDevice);

    bool check_physical_device_properties(VkPhysicalDevice device,
      u32& selectedFamilyIndex,
      u32& swapChainFamilyIndex);

    void create_render_instance();

    void create_screen_surface();

    void create_logical_device();

    void create_swap_chain();

    void get_swap_chain_images();

    void create_cmd_pool();

    void clean_frame_infos();

    void create_frame_infos();

    void release_swap_chain();

    void release_device();

    void release_surface();

    void release_render_instance();

    //Windows
    VkSurfaceKHR m_surface; 
    VkSwapchainKHR m_swapChain;
    VkSurfaceFormatKHR m_swapChainFormat;
    VkSurfaceCapabilitiesKHR m_Capabilities;
    std::vector<VkSurfaceFormatKHR> m_supportedFormats;
    std::vector<VkPresentModeKHR> m_supportedPresentModes;
    //

    // Context state
    VkInstance m_instance;
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;
    VkQueue m_graphicsQueue;
    VkQueue m_presentQueue;
    u32 m_graphicsFamilyIndex;
    u32 m_presentFamilyIndex;
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageView;
    VkCommandPool m_commandPool;
    std::vector<FrameInfoVK> m_frames; // We use N buffering so every frame we will need to use the correct frame info
    
    u32 m_currentFrame;
    u32 m_currentImageIndex;
  };

}
}