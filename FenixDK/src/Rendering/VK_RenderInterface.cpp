#include <algorithm>

#include "VK_RenderInterface.h"
#include "Utilities\platform.h"
#include "Framework\app.h"
#include "Maths\math_utils.h"
#include "Framework\Mesh.h"

// Objects with Vulkan implementation
#include "VK_Buffer.h"
#include "VK_Material.h"
#include "VK_CommandBuffer.h"
#include "Material.h"
#include "Fence.h"
#include "Texture2D.h"

namespace fdk
{
namespace Rendering
{
	// The extensions to use
	static const std::vector<const char*> kInstanceExtensions = {
			// Surface extension to be able to draw into the screen
			VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef WinPlatform
			// This extension is depends on the OS. Makes it possible to create surfaces
			// for Windows
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
	};

#ifdef USE_VALIDATION_LAYERS
	static constexpr u32 kLayerCount = 1;
#else
	static constexpr u32 kLayerCount = 0;
#endif // USE_VALIDATION_LAYERS
	static const char* kLayerNames[] = {"VK_LAYER_LUNARG_standard_validation"};

	static const std::vector<const char*> kDeviceExtensions = {
			// Check that the device supports using swap chains
			VK_KHR_SWAPCHAIN_EXTENSION_NAME};



#ifdef FENIX_VK_IMPL
	RenderInterface* RenderInterface::instance()
	{
		static RenderInterface s_renderInterface;
		return &s_renderInterface;
	}
#endif // FENIX_VK_IMPL

  namespace vulkan_helpers 
  {
    static VkSurfaceFormatKHR get_swap_chain_format(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
      if (availableFormats.size() == 1 &&
        availableFormats[0].format == VK_FORMAT_UNDEFINED) {
        return{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
      }

      for (const VkSurfaceFormatKHR& format : availableFormats)
      {
        if (format.format == VK_FORMAT_R8G8B8A8_UNORM) {
          return format;
        }
      }
      return availableFormats[0];
    }

    VkImageUsageFlags get_swap_chain_flags(VkSurfaceCapabilitiesKHR availableCapabilities)
    {
      if (availableCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
          VK_IMAGE_USAGE_TRANSFER_DST_BIT;
      }
      // Transfer DST usage is not permitted. DST usage is required for clear
      // operations
      return static_cast<VkImageUsageFlags>(-1);
    }

    VkSurfaceTransformFlagBitsKHR get_swap_chain_transform(VkSurfaceCapabilitiesKHR availableCapabilities)
    {
      if (availableCapabilities.supportedTransforms &
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
      }
      return availableCapabilities.currentTransform;
    }

    VkPresentModeKHR get_swap_chain_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
      // Prioritize MAILBOX mode (best option for games: When an image is generated it
      // replaces the image in the queue. So it is always going to show the latest
      // generated image)
      for (auto& mode : availablePresentModes)
      {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
          return mode;
        }
      }
      for (auto& mode : availablePresentModes)
      {
        if (mode == VK_PRESENT_MODE_FIFO_KHR) {
          return mode;
        }
      }
      // Could not find a valid Present mode
      return static_cast<VkPresentModeKHR>(-1);
    }

    u32 find_memory_type_index(const u32 typeFilter, VkMemoryPropertyFlags memProperties, VkPhysicalDevice physicalDevice)
    {
      VkPhysicalDeviceMemoryProperties deviceMemProperties{};
      vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemProperties);
      for (u32 i = 0; i < deviceMemProperties.memoryTypeCount; ++i)
      {
        if ((typeFilter & (1 << i)) && (deviceMemProperties.memoryTypes[i].propertyFlags & memProperties) == memProperties)
        {
          return i;
        }
      }
      FDK_ABORT("Failed to find a type of memory with the given filter and properties");
    }
  } // End vulkan_helpers

	VK_RenderInterface::VK_RenderInterface()
	{
	}
	VK_RenderInterface::~VK_RenderInterface() {}

	void VK_RenderInterface::init()
	{
		create_render_instance();
		create_screen_surface();
		create_logical_device();
		create_swap_chain();
		create_cmd_pool();
		create_frame_infos();
	}

	void VK_RenderInterface::release()
	{
		clean_frame_infos();

		// Wait for the device to be idle
		vkDeviceWaitIdle(m_device);

		release_swap_chain();

		release_device();

		release_device();

		release_surface();

		release_render_instance();
	}

	void VK_RenderInterface::on_resize()
	{
    // TODO: Handle resize
  }

  fdk::Rendering::Texture2D* VK_RenderInterface::get_screen()
  {
    static Texture2D s_screenTexture;
    s_screenTexture.m_view = m_swapChainImageView[m_currentImageIndex];
    return &s_screenTexture;
  }

  void VK_RenderInterface::validate_vk_extensions()
	{
		u32 extensionCount = 0;
		auto result =
				vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		VK_CHECK(result, "Failed to enumerate the extensions of the instance");
		{
			std::vector<VkExtensionProperties> properties(extensionCount);
			result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
																											&properties[0]);
			VK_CHECK(result, "Failed to get the extension properties");

			// Check if all the required extensions are supported
			for (u32 i = 0; i < kInstanceExtensions.size(); i++)
			{
				auto it = std::find_if(properties.begin(), properties.end(),
															 [&](const VkExtensionProperties& prop) {
																 return strcmp(kInstanceExtensions[i],
																							 prop.extensionName) == 0;
															 });
				FDASSERT(it != properties.end(),
									 "Couldn't find the required instance extensions");
			}
		}
	}

	void VK_RenderInterface::check_device_extensions(VkPhysicalDevice pPhysicalDevice)
	{
		u32 extensionCount = 0;
		auto result = vkEnumerateDeviceExtensionProperties(pPhysicalDevice, nullptr,
																											 &extensionCount, nullptr);
		VK_CHECK(result,
						 "Swap chain extensions is not supported in the physical device");
		FDASSERT(extensionCount > 0,
							 "Could not find extensions for the physical device");

		{
			std::vector<VkExtensionProperties> properties(extensionCount);
			result = vkEnumerateDeviceExtensionProperties(
					pPhysicalDevice, nullptr, &extensionCount, &properties[0]);
			VK_CHECK(result,
							 "Failed to enumerate the extensions of the physical device");
			for (const char* name : kDeviceExtensions)
			{
				auto it = std::find_if(properties.begin(), properties.end(),
															 [&name](const VkExtensionProperties& prop) {
																 return strcmp(name, prop.extensionName) == 0;
															 });
				FDASSERT(it != properties.end(),
									 "Couldn't find the required device extensions");
			}
		}
	}

	bool VK_RenderInterface::check_physical_device_properties(VkPhysicalDevice device,
																													 u32& selectedFamilyIndex,
																													 u32& swapChainFamilyIndex)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		u32 majorVersion = VK_VERSION_MAJOR(deviceProperties.apiVersion);
		u32 minorVersion = VK_VERSION_MINOR(deviceProperties.apiVersion);
		u32 patchVersion = VK_VERSION_PATCH(deviceProperties.apiVersion);
		// Check if the version of the API is compatible
		if (majorVersion < 1)
			return false;
		// Check if textures with size greater equal 4096 are allowed
		if (deviceProperties.limits.maxImageDimension2D < 4096)
			return false;
		// We should check everything we want to register later in the logical device

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		u32 familyQueuesCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyQueuesCount, nullptr);
		FDASSERT(familyQueuesCount != 0, "Incorrect count of families found");

		{
			std::vector<VkBool32> swapChainSupport(familyQueuesCount);
			std::vector<VkQueueFamilyProperties> families(familyQueuesCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &familyQueuesCount,
																							 &families[0]);
			for (u32 i = 0; i < familyQueuesCount; i++)
			{
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface,
																						 &swapChainSupport[i]);
				VkQueueFamilyProperties& prop = families[i];
				// We should check different flags for the types of queues we will want to
				// use
				if (prop.queueCount > 0 &&
						prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) // Can queue graphic commands
				{
					if (m_graphicsFamilyIndex == UINT32_MAX) // Not assigned yet
					{
						m_graphicsFamilyIndex = i;
					}

					if (swapChainSupport[i]) {
						m_graphicsFamilyIndex = i;
						m_presentFamilyIndex = i;
						return true;
					}
				}
			}
			// We have not found any family that supports both graphic support and swap
			// chain support
			for (u32 i = 0; i < familyQueuesCount; i++)
			{
				if (swapChainSupport[i]) {
					m_presentFamilyIndex = i;
					break;
				}
			}
			if (m_presentFamilyIndex == UINT32_MAX ||
					m_presentFamilyIndex == UINT32_MAX) {
				return false;
			}
		}
		return true;
	}

	void VK_RenderInterface::create_render_instance()
	{
#ifdef _DEBUG
		validate_vk_extensions();
#endif // _DEBUG

		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = nullptr;
		appInfo.pApplicationName = "Testing Vulkan";
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
		appInfo.pEngineName = Utilities::kEngineName;
		appInfo.engineVersion = Utilities::kEngineVersion;
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo info;
		info.sType = VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = 0;
		info.pApplicationInfo = &appInfo;
		info.enabledLayerCount = kLayerCount;
		info.ppEnabledLayerNames = kLayerNames;
		info.enabledExtensionCount = static_cast<u32>(kInstanceExtensions.size());
		info.ppEnabledExtensionNames = &kInstanceExtensions[0];

		// Create Vulkan instance
		VkResult result = vkCreateInstance(&info, nullptr, &m_instance);
		VK_CHECK(result, "Failed to create VKInstance");
	}

	void VK_RenderInterface::create_screen_surface()
	{
#ifdef WinPlatform
		VkWin32SurfaceCreateInfoKHR surfaceInfo;
		surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		surfaceInfo.pNext = nullptr;
		surfaceInfo.flags = 0;
		surfaceInfo.hinstance = GetModuleHandle(0);
		surfaceInfo.hwnd = GetActiveWindow();

		auto result = vkCreateWin32SurfaceKHR(m_instance, &surfaceInfo, nullptr, &m_surface);
		VK_CHECK(result, "Failed to create Win32 Surface");
#else
		FDK_ABORT("Invalid platform");
#endif
	}

	void VK_RenderInterface::create_logical_device()
	{
		u32 deviceCount = 0;
		auto result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
		VK_CHECK(result, "Failed to get number of physical devices");
		FDASSERT(deviceCount != 0, "Incorrect number of physical devices");

		{
			std::vector<VkPhysicalDevice> devices(deviceCount);
			result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, &devices[0]);
			VK_CHECK(result, "Failed to get physical devices");

			m_graphicsFamilyIndex = UINT32_MAX;
			m_presentFamilyIndex = UINT32_MAX;
			for (u32 i = 0; i < deviceCount; i++)
			{
				if (check_physical_device_properties(
								devices[i], m_graphicsFamilyIndex, m_presentFamilyIndex)) {
					// This device supports graphics and using swap chains
					m_physicalDevice = devices[i];
				}
			}

// Check extensions
#ifdef _DEBUG
			check_device_extensions(m_physicalDevice);
#endif // _DEBUG

			FDASSERT(m_physicalDevice, "A valid physical device was not found");
			static constexpr u32 kPrioritiesCount = 1;
			f32 queuePriorities[kPrioritiesCount] = {1.0f};

			// Queue index 0
			VkDeviceQueueCreateInfo queueInfo;
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.pNext = nullptr;
			queueInfo.flags = 0;
			queueInfo.queueFamilyIndex = m_graphicsFamilyIndex;
			queueInfo.queueCount = kPrioritiesCount;
			queueInfo.pQueuePriorities = queuePriorities;

			std::vector<VkDeviceQueueCreateInfo> queueInfoList;
			queueInfoList.push_back(queueInfo);

			if (m_graphicsFamilyIndex != m_presentFamilyIndex) {
				// Almost the same configuration apart of the family index to use
				queueInfo.queueFamilyIndex = m_presentFamilyIndex;
				queueInfoList.push_back(queueInfo);
			}

			// Features to be enabled, by default it is all disabled
			VkPhysicalDeviceFeatures features{};

			VkDeviceCreateInfo deviceInfo;
			deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceInfo.pNext = nullptr;
			deviceInfo.flags = 0;
			deviceInfo.queueCreateInfoCount = static_cast<u32>(queueInfoList.size());
			deviceInfo.pQueueCreateInfos = &queueInfoList[0];
			deviceInfo.enabledLayerCount = 0;
			deviceInfo.ppEnabledLayerNames = nullptr;
			deviceInfo.enabledExtensionCount =
					static_cast<u32>(kDeviceExtensions.size());
			deviceInfo.ppEnabledExtensionNames = &kDeviceExtensions[0];
			deviceInfo.pEnabledFeatures = &features;
			result = vkCreateDevice(m_physicalDevice, &deviceInfo, nullptr, &m_device);
			VK_CHECK(result, "Failed to create logical device");
		}

		// Get access to the queues we will use
		vkGetDeviceQueue(m_device, m_graphicsFamilyIndex, 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, m_presentFamilyIndex, 0, &m_presentQueue);
	}

	void VK_RenderInterface::create_swap_chain()
	{
		// Get the capabilities of the surface
		auto result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
				m_physicalDevice, m_surface, &m_Capabilities);
		VK_CHECK(result, "Failed to get the capabilities of the surface");
		// Get the range of image formats allowed in the surface
		u32 formatCount = 0;
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface,
																									&formatCount, nullptr);
		VK_CHECK(result, "Failed to enumerate the formats of the surface");
		m_supportedFormats.resize(formatCount);
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(
				m_physicalDevice, m_surface, &formatCount, &m_supportedFormats[0]);
		VK_CHECK(result, "Failed to get the formats of the surface");
		// Get the supported present modes
		u32 presentModeCount = 0;
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(
				m_physicalDevice, m_surface, &presentModeCount, nullptr);
		VK_CHECK(result, "Failed to enumerate the present modes");
		m_supportedPresentModes.resize(presentModeCount);
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(
				m_physicalDevice, m_surface, &presentModeCount,
				&m_supportedPresentModes[0]);
		VK_CHECK(result, "Failed to get the supported present modes");

		// Images extend
		VkExtent2D extend;
		extend.width = Framework::App::width();
		extend.height = Framework::App::height();
		// Get the number of buffers to create in the swap chain
		u32 bufferCount = Maths::clamp(m_Capabilities.minImageCount, m_Capabilities.maxImageCount,
																	 kDesiredSwapChainImageCount);

		// Select a image format to use in the swap chain
		m_swapChainFormat = vulkan_helpers::get_swap_chain_format(m_supportedFormats);
		// Get the transform to apply to the swap chain (Useful in mobiles when using
		// Landscape or portrait)
		VkSurfaceTransformFlagBitsKHR transform = vulkan_helpers::get_swap_chain_transform(m_Capabilities);
		// Get the flags to use in the images of the swap chain
		VkImageUsageFlags usageFlags = vulkan_helpers::get_swap_chain_flags(m_Capabilities);
		// Get the Present mode to use
		VkPresentModeKHR presentMode = vulkan_helpers::get_swap_chain_present_mode(m_supportedPresentModes);

		VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE;
		VkSwapchainCreateInfoKHR swapChainInfo{};
		swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainInfo.pNext = nullptr;
		swapChainInfo.flags = 0;
		swapChainInfo.surface = m_surface;
		swapChainInfo.minImageCount = bufferCount;
		swapChainInfo.imageFormat = m_swapChainFormat.format;
		swapChainInfo.imageColorSpace = m_swapChainFormat.colorSpace;
		swapChainInfo.imageExtent = extend;
		swapChainInfo.imageArrayLayers = 1;
		swapChainInfo.imageUsage = usageFlags;
		swapChainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainInfo.queueFamilyIndexCount = 0;
		swapChainInfo.pQueueFamilyIndices = nullptr;
		swapChainInfo.preTransform = transform;
		swapChainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainInfo.presentMode = presentMode;
		swapChainInfo.clipped = VK_TRUE;
		swapChainInfo.oldSwapchain = oldSwapChain;

		result = vkCreateSwapchainKHR(m_device, &swapChainInfo, nullptr, &m_swapChain);
		VK_CHECK(result, "Failed to create swap chain");
		if (oldSwapChain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(m_device, oldSwapChain, nullptr);
		}
		get_swap_chain_images();
	}

	void VK_RenderInterface::get_swap_chain_images()
	{
		u32 imageCount = 0;
		auto result =
				vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
		VK_CHECK(result, "Failed to enumerate the images of a swap chain");

		m_swapChainImages.resize(imageCount);
		result = vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount,
																		 &m_swapChainImages[0]);
		VK_CHECK(result, "Failed to get the images of the swap chain");

		// Create views for all the images of the swap chain
		m_swapChainImageView.resize(m_swapChainImages.size());
		for (u32 i = 0; i < m_swapChainImages.size(); i++)
		{
			VkImageViewCreateInfo imageViewInfo{};
			imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewInfo.pNext = nullptr;
			imageViewInfo.flags = 0;
			imageViewInfo.image = m_swapChainImages[i];
			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = m_swapChainFormat.format;
			imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewInfo.subresourceRange.baseMipLevel = 0;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.baseArrayLayer = 0;
			imageViewInfo.subresourceRange.layerCount = 1;

			auto result = vkCreateImageView(m_device, &imageViewInfo, nullptr, &m_swapChainImageView[i]);
			VK_CHECK(result, "Failed to create image view");
		}
	}

	void VK_RenderInterface::create_cmd_pool()
	{
		VkCommandPoolCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
								 VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		info.queueFamilyIndex = m_presentFamilyIndex;
		auto result = vkCreateCommandPool(m_device, &info, nullptr, &m_commandPool);
		VK_CHECK(result, "Failed to create command pool");
	}

	void VK_RenderInterface::clean_frame_infos()
	{
		for (FrameInfoVK& frame : m_frames)
		{
			if (frame.finishedRendering)
			{
				vkDestroySemaphore(m_device, frame.finishedRendering, nullptr);
			}
			if (frame.imageAvailable)
			{
				vkDestroySemaphore(m_device, frame.imageAvailable, nullptr);
			}
		}
		m_frames.clear();
	}

	void VK_RenderInterface::create_frame_infos()
	{
		m_currentFrame = 0;
		m_frames.resize(kBufferCount);

		// Semaphore config
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreInfo.pNext = nullptr;
		semaphoreInfo.flags = 0;

		for (FrameInfoVK& frame : m_frames)
		{
			auto result = vkCreateSemaphore(m_device, &semaphoreInfo, nullptr,
																			&frame.finishedRendering);
			VK_CHECK(result, "Failed to create semaphore");
			result = vkCreateSemaphore(m_device, &semaphoreInfo, nullptr,
																 &frame.imageAvailable);
			VK_CHECK(result, "Failed to create semaphore");
		}
	}

	void VK_RenderInterface::release_swap_chain()
	{
		// Destroy the image views of the swap chain
		for (VkImageView& rView : m_swapChainImageView)
		{
			vkDestroyImageView(m_device, rView, nullptr);
		}
		m_swapChainImageView.clear();

		if (m_swapChain)
		{
			vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
		}
	}

	void VK_RenderInterface::release_device()
	{
		if (m_device)
		{
			vkDestroyDevice(m_device, nullptr);
		}
	}

	void VK_RenderInterface::release_surface()
	{
		if (m_surface)
		{
			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		}
	}

	void VK_RenderInterface::release_render_instance()
	{
		if (m_instance)
		{
			vkDestroyInstance(m_instance, nullptr);
		}
	}

  void VK_RenderInterface::create_buffer(Buffer& rBuffer)
  {
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = nullptr;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = rBuffer.size();
    bufferCreateInfo.usage = rBuffer.usage().flags();
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = nullptr;  
    
    auto result = vkCreateBuffer(m_device, &bufferCreateInfo, nullptr, &rBuffer.m_pBuffer);
    VK_CHECK(result, "Failed to create buffer");

    // Get the requirements
    VkMemoryRequirements bufferRequirements{};
    vkGetBufferMemoryRequirements(m_device, rBuffer.m_pBuffer, &bufferRequirements);

    // Allocate the memory
    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = bufferRequirements.size;
    allocateInfo.memoryTypeIndex = 
      vulkan_helpers::find_memory_type_index(bufferRequirements.memoryTypeBits, rBuffer.mem_properties().flags(), m_physicalDevice);
    
    result = vkAllocateMemory(m_device, &allocateInfo, nullptr, &rBuffer.m_pMemory);
    VK_CHECK(result, "Failed to allocate memory");

    // Bind it to the buffer
    vkBindBufferMemory(m_device, rBuffer.m_pBuffer, rBuffer.m_pMemory, 0);
  }

  void VK_RenderInterface::destroy_buffer(Buffer& rBuffer)
  {
    if (rBuffer.m_pBuffer != VK_NULL_HANDLE) 
    {
      vkDestroyBuffer(m_device, rBuffer.m_pBuffer, nullptr);
    }
    if (rBuffer.m_pMemory != VK_NULL_HANDLE)
    {
      vkFreeMemory(m_device, rBuffer.m_pMemory, nullptr);
    }
  }

  void VK_RenderInterface::use_mesh(Framework::Mesh& rMesh, CommandBuffer& rCmdBuffer)
  {
    auto cmdBuffer = rCmdBuffer.m_commandBuffer;
    FDASSERT(cmdBuffer != VK_NULL_HANDLE, "There is not a valid command buffer being used");
    
    Buffer* pVertexBuffer = rMesh.vertex_buffer();
    Buffer* pIndexBuffer = rMesh.index_buffer();
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &pVertexBuffer->m_pBuffer, &offset);
    vkCmdBindIndexBuffer(cmdBuffer, pIndexBuffer->m_pBuffer, offset, VK_INDEX_TYPE_UINT16); // Force indices of 16 bits
  }

  void VK_RenderInterface::bind_material(Material& rMaterial, CommandBuffer& rCmdBuffer)
  {
    auto cmdBuffer = rCmdBuffer.m_commandBuffer;
    FDASSERT(cmdBuffer != VK_NULL_HANDLE, "There is not a valid command buffer being used");
    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, rMaterial.m_pipeline);
  }

  void VK_RenderInterface::set_viewport(const Viewport& rViewport, CommandBuffer& rCmdBuffer)
  {
    auto cmdBuffer = rCmdBuffer.m_commandBuffer;
    FDASSERT(cmdBuffer != VK_NULL_HANDLE, "There is not a valid command buffer being used");
    VkViewport viewport{};
    viewport.x = rViewport.m_offset.x;
    viewport.y = rViewport.m_offset.y;
    viewport.width = rViewport.m_dimensions.x;
    viewport.height = rViewport.m_dimensions.y;
    viewport.minDepth = rViewport.m_minDepth;
    viewport.maxDepth = rViewport.m_maxDepth;
    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
  }

  void VK_RenderInterface::set_scissor(const Scissor& rScissor, CommandBuffer& rCmdBuffer)
  {
    auto cmdBuffer = rCmdBuffer.m_commandBuffer;
    FDASSERT(cmdBuffer != VK_NULL_HANDLE, "There is not a valid command buffer being used");
    VkRect2D scissor{};
    scissor.offset.x = rScissor.m_offsetX;
    scissor.offset.y = rScissor.m_offsetY;
    scissor.extent.width = rScissor.m_width;
    scissor.extent.height = rScissor.m_height;
    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
  }

  void VK_RenderInterface::draw_indexed(const u32 indexCount, const u32 instanceCount, const u32 indexOffset, const u32 vertexOffset, CommandBuffer& rCmdBuffer)
  {
    auto cmdBuffer = rCmdBuffer.m_commandBuffer;
    FDASSERT(cmdBuffer != VK_NULL_HANDLE, "There is not a valid command buffer being used");
    vkCmdDrawIndexed(cmdBuffer, indexCount, instanceCount, indexOffset, vertexOffset, 0);
  }

  void VK_RenderInterface::create_command_buffer(CommandBuffer& rCommandBuffer)
  {
    bool isPrimary = rCommandBuffer.type() == CommandBuffer::kTypePrimary;
    
    VkCommandBufferAllocateInfo info;
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = m_commandPool;
    info.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    info.commandBufferCount = 1;
    auto result = vkAllocateCommandBuffers(m_device, &info, &rCommandBuffer.m_commandBuffer);
    VK_CHECK(result, "Failed to allocate command buffers");
  }

  void* VK_RenderInterface::map_buffer_gpu_memory(Buffer& rBuffer, const u32 memoryOffset, const u32 rangeSize)
  {
    FDASSERT(rBuffer.m_pMemory, "The memory of the buffer has not been allocated");
    FDASSERT(memoryOffset + rangeSize < rBuffer.size(), "Trying to map out of range");
    void* pData = nullptr;
    auto result = vkMapMemory(m_device, rBuffer.m_pMemory, memoryOffset, rangeSize, 0, &pData);
    VK_CHECK(result, "Failed to map memory");
    return pData;
  }

  void VK_RenderInterface::unmap_buffer_gpu_memory(Buffer& rBuffer)
  {
    vkUnmapMemory(m_device, rBuffer.m_pMemory);
  }

  void VK_RenderInterface::create_fence(Fence& rFence)
  {
    // Fence config
    VkFenceCreateInfo fenceInfo;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Signaled by default

    auto result = vkCreateFence(m_device, &fenceInfo, nullptr, &rFence.m_fence);
    VK_CHECK(result, "Failed to create a fence");
  }

  void VK_RenderInterface::beginFrame()
  {
    auto& rFrame = m_frames[m_currentFrame];

    auto result = vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX,
      rFrame.imageAvailable, VK_NULL_HANDLE,
      &m_currentImageIndex);
    switch (result)
    {
    case VK_SUCCESS:
    case VK_SUBOPTIMAL_KHR:
      break;
    case VK_ERROR_OUT_OF_DATE_KHR:
      FDK_ABORT("HANDLE ONRESIZE");
      // HANDLE ON RESIZE
      break;
    }
  }

  void VK_RenderInterface::endFrame()
  {
    auto& rFrame = m_frames[m_currentFrame];

    VkPresentInfoKHR info;
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.pNext = nullptr;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &rFrame.finishedRendering;
    info.swapchainCount = 1;
    info.pSwapchains = &m_swapChain;
    info.pImageIndices = &m_currentImageIndex;
    info.pResults = nullptr;
    auto result = vkQueuePresentKHR(m_presentQueue, &info);
    switch (result)
    {
    case VK_SUCCESS:
      break;
    case VK_ERROR_OUT_OF_DATE_KHR:
    case VK_SUBOPTIMAL_KHR:
      FDK_ABORT("RESIZE WINDOW");
      // TODO Handle Resize
      break;
    default:
      break;
    }

    // Flip command buffers
    m_currentFrame = Maths::wrap(m_currentFrame + 1, 0U, kBufferCount);
  }

  void VK_RenderInterface::submit_work(const CommandBuffer& rCmdBuffer, const Fence& rSyncFence)
  {
    auto& rFrame = m_frames[m_currentFrame];

    VkPipelineStageFlags flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.pNext = nullptr;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &rFrame.imageAvailable;
    info.pWaitDstStageMask = &flags;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &rCmdBuffer.m_commandBuffer;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &rFrame.finishedRendering;
    auto result = vkQueueSubmit(m_presentQueue, 1, &info,
      rSyncFence.m_fence);
    VK_CHECK(result, "Failed to submit cmd block");
  }

}
}
