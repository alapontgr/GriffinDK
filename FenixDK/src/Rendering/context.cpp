#include "context.h"

#include "Framework/app.h"
#include "Maths/math_utils.h"
#include "Utilities/platform.h"
#include <algorithm>
#include <vector>

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

	bool Context::check_physical_device_properties(VkPhysicalDevice device,
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
		FDK_ASSERT(familyQueuesCount != 0, "Incorrect count of families found");

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

	void Context::init()
	{

		create_instance();

		create_surface();

		create_device();

		get_queue();

		create_swap_chain();

		create_cmd_pool();

		allocate_cmd_buffers();

		create_sync_objects();

		init_render_pass();

		create_mesh();

		create_uniform_buffers();

		create_pipeline();
	}

	void Context::check_instance_available_extensions()
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
				FDK_ASSERT(it != properties.end(),
							"Couldn't find the required instance extensions");
			}
		}
	}

	void Context::create_instance()
	{
// Check instance extensions
#ifdef _DEBUG
		check_instance_available_extensions();
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

	void Context::check_device_available_extensions(
			VkPhysicalDevice pPhysicalDevice)
	{
		u32 extensionCount = 0;
		auto result = vkEnumerateDeviceExtensionProperties(pPhysicalDevice, nullptr,
																											 &extensionCount, nullptr);
		VK_CHECK(result,
						 "Swap chain extensions is not supported in the physical device");
		FDK_ASSERT(extensionCount > 0,
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
				FDK_ASSERT(it != properties.end(),
							"Couldn't find the required device extensions");
			}
		}
	}

	VkSurfaceFormatKHR Context::select_swap_chain_format()
	{
		if (m_supportedFormats.size() == 1 &&
				m_supportedFormats[0].format == VK_FORMAT_UNDEFINED) {
			return {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
		}

		for (const VkSurfaceFormatKHR& format : m_supportedFormats)
		{
			if (format.format == VK_FORMAT_R8G8B8A8_UNORM) {
				return format;
			}
		}
		return m_supportedFormats[0];
	}

	VkImageUsageFlags Context::select_swap_chain_flags()
	{
		if (m_Capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
			return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
						 VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}
		// Transfer DST usage is not permitted. DST usage is required for clear
		// operations
		return static_cast<VkImageUsageFlags>(-1);
	}

	VkSurfaceTransformFlagBitsKHR Context::select_swap_chain_transform()
	{
		if (m_Capabilities.supportedTransforms &
				VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
			return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		return m_Capabilities.currentTransform;
	}

	VkPresentModeKHR Context::select_swap_chain_present_mode()
	{
		// Priorice MAILBOX mode (best option for games: When an image is generated it
		// replaces the image in the queue. So it is always going to show the latest
		// generated image)
		for (auto& mode : m_supportedPresentModes)
		{
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return mode;
			}
		}
		for (auto& mode : m_supportedPresentModes)
		{
			if (mode == VK_PRESENT_MODE_FIFO_KHR) {
				return mode;
			}
		}
		// Could not find a valid Present mode
		return static_cast<VkPresentModeKHR>(-1);
	}

	void Context::create_device()
	{
		u32 deviceCount = 0;
		auto result = vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
		VK_CHECK(result, "Failed to get number of physical devices");
		FDK_ASSERT(deviceCount != 0, "Incorrect number of physical devices");

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
			check_device_available_extensions(m_physicalDevice);
#endif // _DEBUG

			FDK_ASSERT(m_physicalDevice, "A valid physical device was not found");
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
	}

	void Context::get_queue()
	{
		// Get access to the queues we will use
		vkGetDeviceQueue(m_device, m_graphicsFamilyIndex, 0, &m_graphicsQueue);
		vkGetDeviceQueue(m_device, m_presentFamilyIndex, 0, &m_presentQueue);
	}

	void Context::create_surface()
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
		ABORT("Invalid platform");
#endif
	}

	void Context::create_sync_objects()
	{
		// Semaphore config
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreInfo.pNext = nullptr;
		semaphoreInfo.flags = 0;

		// Fence config
		VkFenceCreateInfo fenceInfo;
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.pNext = nullptr;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Signaled by default

		for (VirtualFrame& frame : m_cmdBuffers)
		{
			auto result = vkCreateSemaphore(m_device, &semaphoreInfo, nullptr,
																			&frame.finishedRendering);
			VK_CHECK(result, "Failed to create semaphore");
			result = vkCreateSemaphore(m_device, &semaphoreInfo, nullptr,
																 &frame.imageAvailable);
			VK_CHECK(result, "Failed to create semaphore");

			result = vkCreateFence(m_device, &fenceInfo, nullptr, &frame.cmdBufferReady);
			VK_CHECK(result, "Failed to create a fence");
		}
	}

	void Context::create_swap_chain()
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
		u32 bufferCount =
				Maths::clamp(m_Capabilities.minImageCount, m_Capabilities.maxImageCount,
										 kDesiredSwapChainImageCount);
		// Select a image format to use in the swap chain
		m_swapChainFormat = select_swap_chain_format();
		// Get the transform to apply to the swap chain (Useful in mobiles when using
		// Landscape or portrait)
		VkSurfaceTransformFlagBitsKHR transform = select_swap_chain_transform();
		// Get the flags to use in the images of the swap chain
		VkImageUsageFlags usageFlags = select_swap_chain_flags();
		// Get the Present mode to use
		VkPresentModeKHR presentMode = select_swap_chain_present_mode();

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

		result =
				vkCreateSwapchainKHR(m_device, &swapChainInfo, nullptr, &m_swapChain);
		VK_CHECK(result, "Failed to create swap chain");
		if (oldSwapChain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(m_device, oldSwapChain, nullptr);
		}
		check_swap_chain_images();
	}

	void Context::check_swap_chain_images()
	{
		u32 imageCount = 0;
		auto result =
				vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
		VK_CHECK(result, "Failed to enumerate the images of a swap chain");

		{
			m_swapChainImages.resize(imageCount);
			result = vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount,
																			 &m_swapChainImages[0]);
			VK_CHECK(result, "Failed to get the images of the swap chain");
		}
	}

	void Context::create_cmd_pool()
	{
		VkCommandPoolCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.pNext = nullptr;
		info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
								 VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		info.queueFamilyIndex = m_presentFamilyIndex;
		auto result = vkCreateCommandPool(m_device, &info, nullptr, &m_cmdPool);
		VK_CHECK(result, "Failed to create command pool");
	}

	void Context::allocate_cmd_buffers()
	{
		// Use N buffering for the command buffers
		m_cmdBuffers.resize(kBufferCount);

		VkCommandBufferAllocateInfo info;
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.pNext = nullptr;
		info.commandPool = m_cmdPool;
		info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		info.commandBufferCount = 1;
		for (VirtualFrame& frame : m_cmdBuffers)
		{
			auto result = vkAllocateCommandBuffers(m_device, &info, &frame.cmdBuffer);
			VK_CHECK(result, "Failed to allocate command buffers");
		}

		// First command buffer to use
		m_currentCmd = 0;
	}

	void Context::record_cmd_buffer()
	{
		VirtualFrame& frame = m_cmdBuffers[m_currentCmd];

		// Destroy the old framebuffer of the current command buffer and create a new one
		recreate_framebuffer(frame);

		VkCommandBufferBeginInfo info;
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.pNext = nullptr;
		info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		info.pInheritanceInfo = nullptr;

		VkClearValue clearColor = {1.0f, 0.8f, 0.4f, 0.0f};
		VkImageSubresourceRange range;
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		range.baseMipLevel = 0;
		range.levelCount = 1;
		range.baseArrayLayer = 0;
		range.layerCount = 1;

		// begin command buffer
		auto result = vkBeginCommandBuffer(frame.cmdBuffer, &info);
		VK_CHECK(result, "Failed to begin cmd buffer");

		// Record render pass
		bool graphicsNotPresent = m_presentQueue != m_graphicsQueue;
		if (graphicsNotPresent)
		{
			VkImageMemoryBarrier fromPresentToDraw{};
			fromPresentToDraw.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			fromPresentToDraw.pNext = nullptr;
			fromPresentToDraw.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			fromPresentToDraw.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			fromPresentToDraw.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			fromPresentToDraw.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			fromPresentToDraw.srcQueueFamilyIndex = m_presentFamilyIndex;
			fromPresentToDraw.dstQueueFamilyIndex = m_graphicsFamilyIndex;
			fromPresentToDraw.image = m_swapChainImages[m_currentImageIndex];
			fromPresentToDraw.subresourceRange = range;

			vkCmdPipelineBarrier(frame.cmdBuffer,
													 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
													 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
													 0, 0, nullptr, 0, nullptr, 1, &fromPresentToDraw);
		}

		// Begin render pass
		u32 width = Framework::App::width();
		u32 height = Framework::App::height();
		VkRenderPassBeginInfo passBeginInfo{};
		passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passBeginInfo.pNext = nullptr;
		passBeginInfo.renderPass = m_renderPass.handle();
		passBeginInfo.framebuffer = frame.frameBuffer;
		passBeginInfo.renderArea.extent.width = width;
		passBeginInfo.renderArea.extent.height = height;
		passBeginInfo.renderArea.offset.x = 0;
		passBeginInfo.renderArea.offset.y = 0;
		passBeginInfo.clearValueCount = 1;
		passBeginInfo.pClearValues = &clearColor;
		vkCmdBeginRenderPass(frame.cmdBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Bind the correct pipeline depending of shaders, blend states, ...
		vkCmdBindPipeline(frame.cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
		//Set viewport and scissor state
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<f32>(width);
		viewport.height = static_cast<f32>(height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = width;
		scissor.extent.height = height;
		vkCmdSetViewport(frame.cmdBuffer, 0, 1, &viewport);
		vkCmdSetScissor(frame.cmdBuffer, 0, 1, &scissor);

		// Bind vertex buffer
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(frame.cmdBuffer, 0, 1, &m_vertexBuffer.gpuVertexBuffer, &offset);
		vkCmdBindIndexBuffer(frame.cmdBuffer, m_indexBuffer.gpuIndexBuffer, offset, m_indexBuffer.elementType);

		// Bind the descriptor sets
		vkCmdBindDescriptorSets(frame.cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_fragDescSet, 0, nullptr);

		// DRAW all the objects
		vkCmdDrawIndexed(frame.cmdBuffer, m_indexBuffer.elementCount, 1, 0, 0, 0);

		// End render pass
		vkCmdEndRenderPass(frame.cmdBuffer);

		if (graphicsNotPresent)
		{
			VkImageMemoryBarrier fromDrawToPresent{};
			fromDrawToPresent.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			fromDrawToPresent.pNext = nullptr;
			fromDrawToPresent.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			fromDrawToPresent.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
			fromDrawToPresent.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			fromDrawToPresent.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			fromDrawToPresent.srcQueueFamilyIndex = m_graphicsFamilyIndex;
			fromDrawToPresent.dstQueueFamilyIndex = m_presentFamilyIndex;
			fromDrawToPresent.image = m_swapChainImages[m_currentImageIndex];
			fromDrawToPresent.subresourceRange = range;

			vkCmdPipelineBarrier(frame.cmdBuffer,
													 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
													 VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
													 0, 0, nullptr, 0, nullptr, 1, &fromDrawToPresent);
		}

		result = vkEndCommandBuffer(frame.cmdBuffer);
		VK_CHECK(result, "Failed to end command buffer");
	}

	void Context::init_render_pass()
	{
		m_renderPass.init(*this);

		m_swapChainImageView.resize(m_swapChainImages.size());
		for (u32 i = 0; i < m_swapChainImages.size(); i++)
		{
			VkImageViewCreateInfo imageViewInfo{};
			imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewInfo.pNext = nullptr;
			imageViewInfo.flags = 0;
			imageViewInfo.image = m_swapChainImages[i];
			imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewInfo.format = swap_chain_images_format();
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

	void Context::create_pipeline_layout()
	{
		VkPipelineLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.pNext = nullptr;
		layoutInfo.flags = 0;
		layoutInfo.setLayoutCount = 1; // We are using one uniform block in the fragment shader
		layoutInfo.pSetLayouts = &m_fragUniformLayout;
		layoutInfo.pushConstantRangeCount = 0;
		layoutInfo.pPushConstantRanges = nullptr;

		auto result = vkCreatePipelineLayout(m_device, &layoutInfo, nullptr, &m_pipelineLayout);
		VK_CHECK(result, "Failed to create Pipeline Layout");
	}

	void Context::create_pipeline()
	{
		// Load the shaders we are going to use
		m_dummyVS.load(*this, "Shaders/bin/dummy.vert.spv");
		m_dummyPS.load(*this, "Shaders/bin/dummy.frag.spv");

		// Create the descriptor layout for the uniforms
		create_uniform_descriptor_layout();
		create_uniform_descriptor_pool();
		create_uniform_descriptor_set();

		// Create a vector of descriptors for the different stages of the pipeline
		std::vector<VkPipelineShaderStageCreateInfo> shaderStageDesc(2);
		//Vertex shader
		shaderStageDesc[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageDesc[0].pNext = nullptr;
		shaderStageDesc[0].flags = 0;
		shaderStageDesc[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStageDesc[0].module = m_dummyVS.get();
		shaderStageDesc[0].pName = "main";
		shaderStageDesc[0].pSpecializationInfo = nullptr;
		// Pixel shader
		shaderStageDesc[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageDesc[1].pNext = nullptr;
		shaderStageDesc[1].flags = 0;
		shaderStageDesc[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStageDesc[1].module = m_dummyPS.get();
		shaderStageDesc[1].pName = "main";
		shaderStageDesc[0].pSpecializationInfo = nullptr;

		std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions(1);
		vertexBindingDescriptions[0].binding = 0;
		vertexBindingDescriptions[0].stride = sizeof(VertexDesc);
		vertexBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions(2); // One for each parameter in the input layout
		vertexAttributeDescriptions[0].location = 0;
		vertexAttributeDescriptions[0].binding = vertexBindingDescriptions[0].binding;
		vertexAttributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexAttributeDescriptions[0].offset = 0;
		vertexAttributeDescriptions[1].location = 1;
		vertexAttributeDescriptions[1].binding = vertexBindingDescriptions[0].binding;
		vertexAttributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexAttributeDescriptions[1].offset = offsetof(struct VertexDesc, colour);

		VkPipelineVertexInputStateCreateInfo vertexShaderInfo{};
		vertexShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexShaderInfo.pNext = nullptr;
		vertexShaderInfo.flags = 0;
		vertexShaderInfo.vertexBindingDescriptionCount = static_cast<u32>(vertexBindingDescriptions.size());
		vertexShaderInfo.pVertexBindingDescriptions = &vertexBindingDescriptions[0];
		vertexShaderInfo.vertexAttributeDescriptionCount = static_cast<u32>(vertexAttributeDescriptions.size());
		vertexShaderInfo.pVertexAttributeDescriptions = &vertexAttributeDescriptions[0];

		// Define topology to feed the shaders
		VkPipelineInputAssemblyStateCreateInfo assemblerInfo;
		assemblerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assemblerInfo.pNext = nullptr;
		assemblerInfo.flags = 0;
		assemblerInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		assemblerInfo.primitiveRestartEnable = VK_FALSE;

		// Define the viewport to use when drawing
		u32 width = Framework::App::width();
		u32 height = Framework::App::height();
		VkViewport viewport;
		viewport.x = 0.0f; // Left
		viewport.y = 0.0f; // Top
		viewport.width = static_cast<f32>(width);
		viewport.height = static_cast<f32>(height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissors;
		scissors.offset.x = 0;
		scissors.offset.y = 0;
		scissors.extent.width = width;
		scissors.extent.height = height;

		VkPipelineViewportStateCreateInfo viewportStateInfo;
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.pNext = nullptr;
		viewportStateInfo.flags = 0;
		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.pViewports = nullptr; //&viewport;
		viewportStateInfo.scissorCount = 1;
		viewportStateInfo.pScissors = nullptr; //&scissors;

		// Configure rasterizer
		VkPipelineRasterizationStateCreateInfo rasterInfo{};
		rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterInfo.pNext = nullptr;
		rasterInfo.flags = 0;
		rasterInfo.depthClampEnable = VK_FALSE;
		rasterInfo.rasterizerDiscardEnable = VK_FALSE;
		//If we want to use different modes we have to enable the corresponding feature
		rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterInfo.depthBiasEnable = VK_FALSE;
		rasterInfo.depthBiasConstantFactor = 0.0f;
		rasterInfo.depthBiasClamp = 0.0f;
		rasterInfo.depthBiasSlopeFactor = 0.0f;
		rasterInfo.lineWidth = 1.0f;

		// Configure multi sampling state
		VkPipelineMultisampleStateCreateInfo multiSamplingInfo{};
		multiSamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multiSamplingInfo.pNext = nullptr;
		multiSamplingInfo.flags = 0;
		multiSamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multiSamplingInfo.sampleShadingEnable = VK_FALSE;
		multiSamplingInfo.minSampleShading = 1.0f;
		multiSamplingInfo.pSampleMask = nullptr;
		multiSamplingInfo.alphaToCoverageEnable = VK_FALSE;
		multiSamplingInfo.alphaToOneEnable = VK_FALSE;

		// Configure the Blend state
		VkPipelineColorBlendAttachmentState colorBlendState{};
		colorBlendState.blendEnable = VK_FALSE;
		colorBlendState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendState.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendState.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendState.colorWriteMask =
				VK_COLOR_COMPONENT_R_BIT |
				VK_COLOR_COMPONENT_G_BIT |
				VK_COLOR_COMPONENT_B_BIT |
				VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
		colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendInfo.pNext = nullptr;
		colorBlendInfo.flags = 0;
		colorBlendInfo.logicOpEnable = VK_FALSE;
		colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendInfo.attachmentCount = 1;
		colorBlendInfo.pAttachments = &colorBlendState;
		colorBlendInfo.blendConstants[0] = 0.0f;
		colorBlendInfo.blendConstants[1] = 0.0f;
		colorBlendInfo.blendConstants[2] = 0.0f;
		colorBlendInfo.blendConstants[3] = 0.0f;

		// Define dynamic properties
		std::vector<VkDynamicState> dynamicStates{
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR};
		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.pNext = nullptr;
		dynamicStateInfo.flags = 0;
		dynamicStateInfo.dynamicStateCount = static_cast<u32>(dynamicStates.size());
		dynamicStateInfo.pDynamicStates = &dynamicStates[0];

		// Create the pipeline layout (empty)
		create_pipeline_layout();

		// Create the graphic pipeline
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pNext = nullptr;
		pipelineInfo.flags = 0;
		pipelineInfo.stageCount = static_cast<u32>(shaderStageDesc.size());
		pipelineInfo.pStages = &shaderStageDesc[0];
		pipelineInfo.pVertexInputState = &vertexShaderInfo;
		pipelineInfo.pInputAssemblyState = &assemblerInfo;
		pipelineInfo.pTessellationState = nullptr;
		pipelineInfo.pViewportState = &viewportStateInfo;
		pipelineInfo.pRasterizationState = &rasterInfo;
		pipelineInfo.pMultisampleState = &multiSamplingInfo;
		pipelineInfo.pDepthStencilState = nullptr;
		pipelineInfo.pColorBlendState = &colorBlendInfo;
		pipelineInfo.pDynamicState = &dynamicStateInfo;
		pipelineInfo.layout = m_pipelineLayout;
		pipelineInfo.renderPass = m_renderPass.handle();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		auto result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline);
		VK_CHECK(result, "Failed to create Graphics Pipeline");
	}

	void Context::create_uniform_descriptor_layout()
	{
		// Create the description of the uniforms
		VkDescriptorSetLayoutBinding descSetLayout{};
		descSetLayout.binding = 0; // The slot where this descriptor is going to be bound in the shader
		descSetLayout.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descSetLayout.descriptorCount = 1;											 // It is possible to create an array of uniform blocks
		descSetLayout.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; // In our example we want parameters in the fragment shader
		descSetLayout.pImmutableSamplers = nullptr;

		// Create the layout
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.pNext = nullptr;
		layoutInfo.flags = 0;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &descSetLayout;

		auto result = vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &m_fragUniformLayout);
		VK_CHECK(result, "Failed to create the descriptor set layout");
	}

	void Context::create_uniform_descriptor_pool()
	{
		VkDescriptorPoolSize descPoolSize{};
		descPoolSize.descriptorCount = 1;
		descPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; // We are using a uniform buffer object in the fragment shader

		VkDescriptorPoolCreateInfo descPoolInfo{};
		descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descPoolInfo.pNext = nullptr;
		descPoolInfo.flags = 0;
		descPoolInfo.poolSizeCount = 1;
		descPoolInfo.pPoolSizes = &descPoolSize;
		descPoolInfo.maxSets = 1;

		auto result = vkCreateDescriptorPool(m_device, &descPoolInfo, nullptr, &m_fragDescPool);
		VK_CHECK(result, "Failed to create the descriptor pool");
	}

	void Context::create_uniform_descriptor_set()
	{
		VkDescriptorSetAllocateInfo descSetAllocInfo{};
		descSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descSetAllocInfo.pNext = nullptr;
		descSetAllocInfo.descriptorPool = m_fragDescPool;
		descSetAllocInfo.descriptorSetCount = 1;
		descSetAllocInfo.pSetLayouts = &m_fragUniformLayout;
		auto result = vkAllocateDescriptorSets(m_device, &descSetAllocInfo, &m_fragDescSet);
		VK_CHECK(result, "Failed to allocate the descriptor set");

		// Bind the resources
		VkDescriptorBufferInfo descBuffInfo{};
		descBuffInfo.buffer = m_fragUniformBuffer.pBuffer;
		descBuffInfo.offset = 0;
		descBuffInfo.range = m_fragUniformBuffer.size;

		VkWriteDescriptorSet writeDescSet{};
		writeDescSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescSet.pNext = nullptr;
		writeDescSet.dstSet = m_fragDescSet;
		writeDescSet.dstBinding = 0; // The slot where we want to bind the resource
		writeDescSet.dstArrayElement = 0;
		writeDescSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescSet.descriptorCount = 1;
		writeDescSet.pBufferInfo = &descBuffInfo;
		writeDescSet.pImageInfo = nullptr;
		writeDescSet.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(m_device, 1, &writeDescSet, 0, nullptr);
	}

	u32 Context::find_memory_type_index(const u32 typeFilter, VkMemoryPropertyFlags memProperties)
	{
		VkPhysicalDeviceMemoryProperties deviceMemProperties{};
		vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &deviceMemProperties);
		for (u32 i = 0; i < deviceMemProperties.memoryTypeCount; ++i)
		{
			if ((typeFilter & (1 << i)) && (deviceMemProperties.memoryTypes[i].propertyFlags & memProperties) == memProperties)
			{
				return i;
			}
		}
		ABORT("Failed to find a type of memory with the given filter and properties");
	}

	void Context::create_buffer(
			VkDeviceSize bufferSize,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryProperties,
			VkBuffer& rOutBuffer,
			VkDeviceMemory& rOutMemory)
	{
		// Create the buffer
		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.size = bufferSize;
		createInfo.usage = usageFlags;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;

		auto result = vkCreateBuffer(m_device, &createInfo, nullptr, &rOutBuffer);
		VK_CHECK(result, "Failed to create buffer");

		// Get the requirements
		VkMemoryRequirements bufferRequirements{};
		vkGetBufferMemoryRequirements(m_device, rOutBuffer, &bufferRequirements);

		// Allocate the memory
		VkMemoryAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.pNext = nullptr;
		allocateInfo.allocationSize = bufferRequirements.size;
		allocateInfo.memoryTypeIndex = find_memory_type_index(bufferRequirements.memoryTypeBits, memoryProperties);
		result = vkAllocateMemory(m_device, &allocateInfo, nullptr, &rOutMemory);
		VK_CHECK(result, "Failed to allocate memory");

		// Bind it to the buffer
		vkBindBufferMemory(m_device, rOutBuffer, rOutMemory, 0);
	}

	void Context::copy_buffer(VkBuffer& rFrom, VkBuffer& rTo, const u32 size)
	{
		VkCommandBufferAllocateInfo bufferAllocateInfo{};
		bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		bufferAllocateInfo.pNext = nullptr;
		bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		bufferAllocateInfo.commandPool = m_cmdPool;
		bufferAllocateInfo.commandBufferCount = 1;

		// Create a temporary command buffer to execute the command
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
		vkAllocateCommandBuffers(m_device, &bufferAllocateInfo, &commandBuffer);

		VkCommandBufferBeginInfo cmdBufferBegin{};
		cmdBufferBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufferBegin.pNext = nullptr;
		cmdBufferBegin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		cmdBufferBegin.pInheritanceInfo = nullptr;

		// Begin recording
		vkBeginCommandBuffer(commandBuffer, &cmdBufferBegin);

		// Queue the copy command
		VkBufferCopy buffCopy{};
		buffCopy.srcOffset = 0;
		buffCopy.dstOffset = 0;
		buffCopy.size = size;
		vkCmdCopyBuffer(commandBuffer, rFrom, rTo, 1, &buffCopy);

		// End recording
		vkEndCommandBuffer(commandBuffer);

		// Submit to the GPU
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_graphicsQueue);

		// Free the command buffer
		vkFreeCommandBuffers(m_device, m_cmdPool, 1, &commandBuffer);
	}

	void Context::create_mesh()
	{
		create_vertex_buffer();
		create_index_buffer();
	}

	void Context::create_vertex_buffer()
	{
		VertexDesc vertices[] =
				{
						{v4(-0.5f, -0.5f, 0.0f, 1.0f),
						 v4(1.0f, 0.0f, 0.0f, 1.0f)},
						{v4(0.5f, -0.5f, 0.0f, 1.0f),
						 v4(1.0f, 1.0f, 0.0f, 1.0f)},
						{v4(0.5f, 0.5f, 0.0f, 1.0f),
						 v4(0.0f, 1.0f, 1.0f, 1.0f)},
						{v4(-0.5f, 0.5f, 0.0f, 1.0f),
						 v4(0.0f, 0.0f, 1.0f, 1.0f)},
				};

		m_vertexBuffer.size = 4 * sizeof(VertexDesc);

		// Create the staging buffer and allocate it
		create_buffer(
				m_vertexBuffer.size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_vertexBuffer.stagingBuffer,
				m_vertexBuffer.pStagingBufferMem);

		// Upload data to the staging buffer
		void* pData = nullptr;
		auto result = vkMapMemory(m_device, m_vertexBuffer.pStagingBufferMem, 0, m_vertexBuffer.size, 0, &pData);
		VK_CHECK(result, "Failed tyo map memory");
		memcpy(pData, vertices, m_vertexBuffer.size);
		vkUnmapMemory(m_device, m_vertexBuffer.pStagingBufferMem);

#ifdef DEAD
		VkMappedMemoryRange memRange{};
		memRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		memRange.pNext = nullptr;
		memRange.memory = m_vertexBuffer.pStagingBufferMem;
		memRange.offset = 0;
		memRange.size = VK_WHOLE_SIZE;
		vkFlushMappedMemoryRanges(m_device, 1, &memRange); // Flush data to the gpu
#endif																								 // _DEBUG

		create_buffer(
				m_vertexBuffer.size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_vertexBuffer.gpuVertexBuffer,
				m_vertexBuffer.pGpuVertexBufferMem);

		// Copy the data to the final vertex buffer
		copy_buffer(m_vertexBuffer.stagingBuffer, m_vertexBuffer.gpuVertexBuffer, m_vertexBuffer.size);
	}

	void Context::create_index_buffer()
	{
		u16 indices[] =
				{
						0, 1, 2, 2, 3, 0};

		m_indexBuffer.elementCount = 6;
		m_indexBuffer.size = m_indexBuffer.elementCount * sizeof(u16);
		m_indexBuffer.elementType = VK_INDEX_TYPE_UINT16;

		// Create the staging buffer and allocate it
		create_buffer(
				m_indexBuffer.size,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_indexBuffer.stagingBuffer,
				m_indexBuffer.pStagingBufferMem);

		// Upload data to the staging buffer
		void* pData = nullptr;
		auto result = vkMapMemory(m_device, m_indexBuffer.pStagingBufferMem, 0, m_indexBuffer.size, 0, &pData);
		VK_CHECK(result, "Failed tyo map memory");
		memcpy(pData, indices, m_indexBuffer.size);
		vkUnmapMemory(m_device, m_indexBuffer.pStagingBufferMem);

#ifdef DEAD
		VkMappedMemoryRange memRange{};
		memRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		memRange.pNext = nullptr;
		memRange.memory = m_indexBuffer.pStagingBufferMem;
		memRange.offset = 0;
		memRange.size = VK_WHOLE_SIZE;
		vkFlushMappedMemoryRanges(m_device, 1, &memRange); // Flush data to the gpu
#endif																								 // DEAD

		create_buffer(
				m_indexBuffer.size,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_indexBuffer.gpuIndexBuffer,
				m_indexBuffer.pGpuIndexBufferMem);

		// Copy the data to the final vertex buffer
		copy_buffer(m_indexBuffer.stagingBuffer, m_indexBuffer.gpuIndexBuffer, m_indexBuffer.size);
	}

	void Context::on_resize_window()
	{
		clean_cmd_buffers();
		create_swap_chain();
		create_cmd_pool();
		allocate_cmd_buffers();
	}

	void Context::acquire_next_free_image()
	{
		VirtualFrame& frame = m_cmdBuffers[m_currentCmd];

		// Wait for the active command buffer using its fence
		auto result = vkWaitForFences(m_device, 1, &frame.cmdBufferReady, VK_FALSE, 1000000000);
		VK_CHECK(result, "Waited to long for fences");
		result = vkResetFences(m_device, 1, &frame.cmdBufferReady);
		VK_CHECK(result, "Failed to reset the fences");

		result = vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX,
																	 frame.imageAvailable, VK_NULL_HANDLE,
																	 &m_currentImageIndex);
		switch (result)
		{
		case VK_SUCCESS:
		case VK_SUBOPTIMAL_KHR:
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			on_resize_window();
			break;
		}
	}

	void Context::recreate_framebuffer(VirtualFrame& frame)
	{
		// Destroy the old one
		if (frame.frameBuffer)
		{
			vkDestroyFramebuffer(m_device, frame.frameBuffer, nullptr);
			frame.frameBuffer = VK_NULL_HANDLE;
		}

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.pNext = nullptr;
		framebufferInfo.flags = 0;
		framebufferInfo.renderPass = m_renderPass.handle();
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &m_swapChainImageView[m_currentImageIndex];
		framebufferInfo.width = Framework::App::width();
		framebufferInfo.height = Framework::App::height();
		framebufferInfo.layers = 1;

		auto result = vkCreateFramebuffer(m_device, &framebufferInfo, VK_NULL_HANDLE, &frame.frameBuffer);
		VK_CHECK(result, "Failed to create framebuffer");
	}

	void Context::submit_cmd_buffer()
	{
		VirtualFrame& frame = m_cmdBuffers[m_currentCmd];

		VkPipelineStageFlags flags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		VkSubmitInfo info{};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.pNext = nullptr;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &frame.imageAvailable;
		info.pWaitDstStageMask = &flags;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &frame.cmdBuffer;
		info.signalSemaphoreCount = 1;
		info.pSignalSemaphores = &frame.finishedRendering;
		auto result = vkQueueSubmit(m_presentQueue, 1, &info,
																frame.cmdBufferReady);
		VK_CHECK(result, "Failed to submit cmd block");
	}

	void Context::clean_cmd_buffers()
	{
		if (m_device) {
			// Wait until the device is not using any resources and is idle
			vkDeviceWaitIdle(m_device);

			for (VirtualFrame& frame : m_cmdBuffers)
			{
				if (frame.cmdBuffer)
				{
					vkFreeCommandBuffers(m_device, m_cmdPool, 1, &frame.cmdBuffer);
				}
				if (frame.imageAvailable)
				{
					vkDestroySemaphore(m_device, frame.imageAvailable, nullptr);
				}
				if (frame.finishedRendering)
				{
					vkDestroySemaphore(m_device, frame.finishedRendering, nullptr);
				}
				if (frame.cmdBufferReady)
				{
					vkDestroyFence(m_device, frame.cmdBufferReady, nullptr);
				}
				if (frame.frameBuffer)
				{
					vkDestroyFramebuffer(m_device, frame.frameBuffer, nullptr);
				}
			}
			m_cmdBuffers.clear();
			if (m_cmdPool) {
				vkDestroyCommandPool(m_device, m_cmdPool, nullptr);
			}
		}
	}

	void Context::release()
	{
		clean_cmd_buffers();
		// Release resources in the inverse order they were created
		if (m_device) {
			// Wait until the device is not using any resources and is idle
			vkDeviceWaitIdle(m_device);

      // Release the meshes
			release_mesh();

			// Destroy the image views of the swap chain
			for (VkImageView& rView : m_swapChainImageView)
			{
				vkDestroyImageView(m_device, rView, nullptr);
			}
			m_swapChainImageView.clear();

			// Destroy the pipeline
			if (m_pipeline)
			{
				vkDestroyPipeline(m_device, m_pipeline, nullptr);
			}
			if (m_pipelineLayout)
			{
				vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
			}

			clean_materials();

			if (m_swapChain) {
				vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
			}

			vkDestroyDevice(m_device, nullptr);
		}

		if (m_surface) {
			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		}

		if (m_instance) {
			vkDestroyInstance(m_instance, nullptr);
		}
	}

	void Context::clean_materials()
	{
		// Destroy the render pass
		m_renderPass.release(*this);
		// Destroy the shaders
		m_dummyVS.release();
		m_dummyPS.release();
		// Destroy the descriptor sets
		if (m_fragUniformLayout)
		{
			vkDestroyDescriptorSetLayout(m_device, m_fragUniformLayout, nullptr);
		}

		// Destroy the uniforms
		if (m_fragUniformBuffer.pBuffer)
		{
			vkDestroyBuffer(m_device, m_fragUniformBuffer.pBuffer, nullptr);
		}
		if (m_fragUniformBuffer.pMemory)
		{
			vkFreeMemory(m_device, m_fragUniformBuffer.pMemory, nullptr);
		}

		if (m_fragDescPool)
		{
			vkDestroyDescriptorPool(m_device, m_fragDescPool, nullptr);
		}
	}

	void Context::begin_scene(const v4& clearColor)
	{
		acquire_next_free_image();
	}

	void Context::test_render(const f64 totalTime)
	{
		// Update Uniforms
		update_uniforms(totalTime);

		// Render
		record_cmd_buffer();
		submit_cmd_buffer();
	}

	void Context::end_scene()
	{
		VirtualFrame& frame = m_cmdBuffers[m_currentCmd];

		VkPresentInfoKHR info;
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.pNext = nullptr;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &frame.finishedRendering;
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
			on_resize_window();
			break;
		default:
			break;
		}

		// Flip command buffers
		m_currentCmd = Maths::wrap(m_currentCmd + 1, 0U, kBufferCount);
	}

	void Context::release_mesh()
	{
		// Destroy the buffers linked to the mesh
		if (m_vertexBuffer.stagingBuffer)
		{
			vkDestroyBuffer(m_device, m_vertexBuffer.stagingBuffer, nullptr);
		}
		if (m_vertexBuffer.pStagingBufferMem)
		{
			vkFreeMemory(m_device, m_vertexBuffer.pStagingBufferMem, nullptr);
		}
		if (m_vertexBuffer.gpuVertexBuffer)
		{
			vkDestroyBuffer(m_device, m_vertexBuffer.gpuVertexBuffer, nullptr);
		}
		if (m_vertexBuffer.pGpuVertexBufferMem)
		{
			vkFreeMemory(m_device, m_vertexBuffer.pGpuVertexBufferMem, nullptr);
		}

		if (m_indexBuffer.stagingBuffer)
		{
			vkDestroyBuffer(m_device, m_indexBuffer.stagingBuffer, nullptr);
		}
		if (m_indexBuffer.pStagingBufferMem)
		{
			vkFreeMemory(m_device, m_indexBuffer.pStagingBufferMem, nullptr);
		}
		if (m_indexBuffer.gpuIndexBuffer)
		{
			vkDestroyBuffer(m_device, m_indexBuffer.gpuIndexBuffer, nullptr);
		}
		if (m_indexBuffer.pGpuIndexBufferMem)
		{
			vkFreeMemory(m_device, m_indexBuffer.pGpuIndexBufferMem, nullptr);
		}
	}

	void Context::create_uniform_buffers()
	{
		m_fragUniformBuffer.size = sizeof(FragShaderUniforms);
		create_buffer(
				m_fragUniformBuffer.size,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				m_fragUniformBuffer.pBuffer,
				m_fragUniformBuffer.pMemory);
	}

	void Context::update_uniforms(const f64 totalTime)
	{
		f32 fTime = static_cast<f32>(totalTime);
		f32 s = (sin(fTime) + 1.0f) * 0.5f;
		f32 c = (cos(fTime) + 1.0f) * 0.5f;
		FragShaderUniforms ubo;
		ubo.colour = v4(s, c, s, 1.0f);

		void* pData = nullptr;
		auto result = vkMapMemory(m_device, m_fragUniformBuffer.pMemory, 0, m_fragUniformBuffer.size, 0, &pData);
		VK_CHECK(result, "Failed to map memory");
		memcpy(pData, &ubo, m_fragUniformBuffer.size);
		vkUnmapMemory(m_device, m_fragUniformBuffer.pMemory);

#ifdef DEAD
		VkMappedMemoryRange memRange{};
		memRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		memRange.pNext = nullptr;
		memRange.memory = m_fragUniformBuffer.pMemory;
		memRange.offset = 0;
		memRange.size = VK_WHOLE_SIZE;
		vkFlushMappedMemoryRanges(m_device, 1, &memRange); // Flush data to the gpu
#endif // _DEBUG
	}
}
}