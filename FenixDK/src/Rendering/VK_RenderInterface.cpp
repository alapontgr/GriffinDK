#include <algorithm>

#include "VK_RenderInterface.h"
#include "Utilities\platform.h"
#include "Framework\app.h"
#include "Maths\math_utils.h"

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

	RenderInterface* RenderInterface::create()
	{
		static VKRenderInterface s_renderInterface;
		return &s_renderInterface;
	}

	VKRenderInterface::VKRenderInterface() {}
	VKRenderInterface::~VKRenderInterface() {}

	void VKRenderInterface::init()
	{
		create_render_instance();
		create_screen_surface();
		create_logical_device();
    create_swap_chain();
    create_cmd_pool();
    create_frame_infos();
	}

	void VKRenderInterface::release()
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

  void VKRenderInterface::on_resize()
  {
    m_flags.set(kPendingResize, true);
    create_swap_chain();
  }

  void VKRenderInterface::validate_vk_extensions()
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

  void VKRenderInterface::check_device_extensions(VkPhysicalDevice pPhysicalDevice)
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

  bool VKRenderInterface::check_physical_device_properties(VkPhysicalDevice device,
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

  void VKRenderInterface::create_render_instance()
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

	void VKRenderInterface::create_screen_surface()
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

	void VKRenderInterface::create_logical_device()
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
      check_device_extensions(m_physicalDevice);
#endif // _DEBUG

      FDK_ASSERT(m_physicalDevice, "A valid physical device was not found");
      static constexpr u32 kPrioritiesCount = 1;
      f32 queuePriorities[kPrioritiesCount] = { 1.0f };

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

  void VKRenderInterface::create_swap_chain()
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
    m_swapChainFormat = get_swap_chain_format(m_supportedFormats);
    // Get the transform to apply to the swap chain (Useful in mobiles when using
    // Landscape or portrait)
    VkSurfaceTransformFlagBitsKHR transform = get_swap_chain_transform(m_Capabilities);
    // Get the flags to use in the images of the swap chain
    VkImageUsageFlags usageFlags = get_swap_chain_flags(m_Capabilities);
    // Get the Present mode to use
    VkPresentModeKHR presentMode = get_swap_chain_present_mode(m_supportedPresentModes);

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

  void VKRenderInterface::get_swap_chain_images()
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

  void VKRenderInterface::create_cmd_pool()
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

  void VKRenderInterface::clean_frame_infos()
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

  void VKRenderInterface::create_frame_infos()
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

  void VKRenderInterface::release_swap_chain()
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

  void VKRenderInterface::release_device()
  {
    if (m_device) 
    {
      vkDestroyDevice(m_device, nullptr);
    }
  }

  void VKRenderInterface::release_surface()
  {
    if (m_surface) 
    {
      vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    }
  }

  void VKRenderInterface::release_render_instance()
  {
    if (m_instance) 
    {
      vkDestroyInstance(m_instance, nullptr);
    }
  }

}
}
