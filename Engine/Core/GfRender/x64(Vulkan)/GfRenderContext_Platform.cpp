////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/15
//	File: 	GFRENDERCONTEXT_PLATFORM.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfRenderContext.h"
#include "GfRender/Common/GfWindow.h"
#include GF_SOLVE_PLATFORM_HEADER(GfRender)
#include "GfCore/Common/GfMaths.h"

#include <vector>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////

#define GF_N_BUFFERING_COUNT 2

////////////////////////////////////////////////////////////////////////////////

// The extensions to use
static u32 g_uiInstanceExtensionCount(2); 
static const char* g_pInstanceExtensions[] = 
{
	// Surface extension to be able to draw into the screen
	VK_KHR_SURFACE_EXTENSION_NAME,

	// This extension is depends on the OS. Makes it possible to create surfaces
	// for Windows
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
};

// Validation layers
#if GF_USE_VALIDATION_LAYERS
static constexpr u32 g_uiLayerCount = 1;
#else
static constexpr u32 g_uiLayerCount = 0;
#endif // USE_VALIDATION_LAYERS
static const char* g_pLayerNames[] = 
{ 
	"VK_LAYER_LUNARG_standard_validation" 
};

// Device extensions
static constexpr u32 g_uiDeviceExtensionCount(1);
static const char* g_pDeviceExtensions[] = 
{
	// Check that the device supports using swap chains
	VK_KHR_SWAPCHAIN_EXTENSION_NAME 
};

////////////////////////////////////////////////////////////////////////////////
// Functions

////////////////////////////////////////////////////////////////////////////////

GfRenderContext_Platform::GfRenderContext_Platform(GfRenderContext& kBase)
	: m_kBase(kBase)
	, m_pInstance(nullptr)
	, m_pPhysicalDevice(nullptr)
	, m_pDevice(nullptr)
	, m_pSurface(0)
	, m_pSwapChain(0)
	, m_uiCurrentImageIdx(0)
{
	for (u32 i=0; i<GfRencerContextFamilies::Count; ++i) 
	{
		m_pQueues[i] = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

VkImageView GfRenderContext_Platform::GetCurrentBackBufferView() const
{
	return m_tSwapChainImageView[m_uiCurrentImageIdx];
}

////////////////////////////////////////////////////////////////////////////////

VkImage GfRenderContext_Platform::GetCurrentBackBuffer() const
{
	return m_tSwapChainImages[m_uiCurrentImageIdx];
}

////////////////////////////////////////////////////////////////////////////////

const GfFrameSyncing& GfRenderContext_Platform::GetFrameSyncPrimitives() const
{
	return m_pFrameSyncEntries[m_kBase.m_uiCurrentFrameIdx];
}

////////////////////////////////////////////////////////////////////////////////

VkFormat GfRenderContext_Platform::GetSwapchainFormat() const
{
	return m_kSwapChainFormat.format;
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::InitInternal()
{
	CreateInstance();
	CreateSurface();
	CreateDevice();
	RetrieveQueues();
	CreateSwapchain();
	CreateSyncPrimitives();
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::BeginFramePlatform()
{
	const GfFrameSyncing& kSyncPrimitives(GetFrameSyncPrimitives());
	VkResult eResult = vkAcquireNextImageKHR(m_pDevice, m_pSwapChain, UINT64_MAX,
		kSyncPrimitives.m_pImageReady, VK_NULL_HANDLE,
		&m_uiCurrentImageIdx);
	switch (eResult)
	{
	case VK_SUCCESS:
	case VK_SUBOPTIMAL_KHR:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
		m_kBase.OnResize();
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::EndFramePlatform()
{
	const GfFrameSyncing& kSyncPrimitives(GetFrameSyncPrimitives());

	VkPresentInfoKHR kInfo;
	kInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	kInfo.pNext = nullptr;
	kInfo.waitSemaphoreCount = 1;
	kInfo.pWaitSemaphores = &kSyncPrimitives.m_pFinishedRendering;
	kInfo.swapchainCount = 1;
	kInfo.pSwapchains = &m_pSwapChain;
	kInfo.pImageIndices = &m_uiCurrentImageIdx;
	kInfo.pResults = nullptr;
	auto result = vkQueuePresentKHR(GetQueue(GfRencerContextFamilies::Present), &kInfo);
	switch (result)
	{
	case VK_SUCCESS:
		break;
	case VK_ERROR_OUT_OF_DATE_KHR:
	case VK_SUBOPTIMAL_KHR:
		m_kBase.OnResize();
		break;
	default:
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////

bool GfRenderContext_Platform::CheckPhysicalDeviceProperties(
	VkPhysicalDevice pDevice,
	u32& uiOutGraphicsFamilyIndex,
	u32& uiOutPresentFamilyIndex)
{
	VkPhysicalDeviceProperties kDeviceProperties;
	vkGetPhysicalDeviceProperties(pDevice, &kDeviceProperties);

	u32 uiMajorVersion = VK_VERSION_MAJOR(kDeviceProperties.apiVersion);
	u32 uiMinorVersion = VK_VERSION_MINOR(kDeviceProperties.apiVersion);
	u32 uiPatchVersion = VK_VERSION_PATCH(kDeviceProperties.apiVersion);
	// Check if the version of the API is compatible
	if (uiMajorVersion < 1)
		return false;
	// Check if textures with size greater equal 4096 are allowed
	if (kDeviceProperties.limits.maxImageDimension2D < 4096)
		return false;
	// We should check everything we want to register later in the logical device

	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(pDevice, &deviceFeatures);

	u32 uiFamilyQueuesCount(0);
	vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &uiFamilyQueuesCount, nullptr);
	GF_ASSERT(uiFamilyQueuesCount != 0, "Incorrect count of families found");
	{
		std::vector<VkBool32> tSwapChainSupport(uiFamilyQueuesCount);
		std::vector<VkQueueFamilyProperties> tFamilies(uiFamilyQueuesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &uiFamilyQueuesCount, &tFamilies[0]);
		for (u32 i = 0; i < uiFamilyQueuesCount; i++)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(pDevice, i, m_pSurface, &tSwapChainSupport[i]);
			VkQueueFamilyProperties& kProp(tFamilies[i]);

			// We should check different flags for the types of queues we will want to use
			if (kProp.queueCount > 0 &&
				kProp.queueFlags & VK_QUEUE_GRAPHICS_BIT) // Can queue graphic commands
			{
				if (uiOutGraphicsFamilyIndex == UINT32_MAX) // Not assigned yet
				{
					uiOutGraphicsFamilyIndex = i;
				}

				if (tSwapChainSupport[i]) {
					uiOutGraphicsFamilyIndex = i;
					uiOutPresentFamilyIndex = i;
					return true;
				}
			}
		}
		// We have not found any family that supports both graphic support and swap
		// chain support
		for (u32 i = 0; i < uiFamilyQueuesCount; i++)
		{
			if (tSwapChainSupport[i]) {
				uiOutPresentFamilyIndex = i;
				break;
			}
		}
		if (uiOutPresentFamilyIndex == UINT32_MAX ||
			uiOutPresentFamilyIndex == UINT32_MAX) 
		{
			return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::CheckValidationLayerSupport()
{
	u32 uiLayerCount;
	vkEnumerateInstanceLayerProperties(&uiLayerCount, nullptr);

	std::vector<VkLayerProperties> tAvailableLayers(uiLayerCount);
	vkEnumerateInstanceLayerProperties(&uiLayerCount, tAvailableLayers.data());
	for (const char* szLayerName : g_pLayerNames)
	{
		bool bLayerFound = false;
		for (const auto& layerProperties : tAvailableLayers)
		{
			if (strcmp(szLayerName, layerProperties.layerName) == 0) {
				bLayerFound = true;
				break;
			}
		}
		GF_ASSERT(bLayerFound, "Failed to get validation layers");
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::CheckInstanceAvalExtensions()
{
	u32 uiExtensionCount = 0;
	VkResult eResult = vkEnumerateInstanceExtensionProperties(nullptr, &uiExtensionCount, nullptr);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to enumerate the extensions of the instance");
	{
		std::vector<VkExtensionProperties> tProperties(uiExtensionCount);
		eResult = vkEnumerateInstanceExtensionProperties(nullptr, &uiExtensionCount,
			&tProperties[0]);
		GF_ASSERT(eResult == VK_SUCCESS, "Failed to get the extension properties");

		// Check if all the required extensions are supported
		for (u32 i = 0; i < g_uiInstanceExtensionCount; i++)
		{
			auto it = std::find_if(tProperties.begin(), tProperties.end(),
				[&](const VkExtensionProperties& prop) {
				return strcmp(g_pInstanceExtensions[i],	prop.extensionName) == 0;
			});
			GF_ASSERT(it != tProperties.end(), "Couldn't find the required instance extensions");
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::CheckDeviceAvalExtensions(VkPhysicalDevice pPhysicalDevice)
{

}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::CheckSwapchainImages()
{
	u32 uiImageCount(0);
	VkResult eResult = vkGetSwapchainImagesKHR(m_pDevice, m_pSwapChain, &uiImageCount, nullptr);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to enumerate the images of a swap chain");

	// Get the Images
	{
		m_tSwapChainImages.resize(uiImageCount);
		eResult = vkGetSwapchainImagesKHR(m_pDevice, m_pSwapChain, &uiImageCount,
			&m_tSwapChainImages[0]);
		GF_ASSERT(eResult == VK_SUCCESS, "Failed to get the images of the swap chain");
	}

	// Create the Image views for the back buffer
	{
		m_tSwapChainImageView.resize(uiImageCount);

		for (u32 i = 0; i < uiImageCount; i++)
		{
			VkImageViewCreateInfo kImageViewInfo{};
			kImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			kImageViewInfo.pNext = nullptr;
			kImageViewInfo.flags = 0;
			kImageViewInfo.image = m_tSwapChainImages[i];
			kImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			kImageViewInfo.format = m_kSwapChainFormat.format;
			kImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			kImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			kImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			kImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			kImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			kImageViewInfo.subresourceRange.baseMipLevel = 0;
			kImageViewInfo.subresourceRange.levelCount = 1;
			kImageViewInfo.subresourceRange.baseArrayLayer = 0;
			kImageViewInfo.subresourceRange.layerCount = 1;

			VkResult eResult = vkCreateImageView(m_pDevice, &kImageViewInfo, nullptr, &m_tSwapChainImageView[i]);
			GF_ASSERT(eResult == VK_SUCCESS, "Failed to create image view");
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

VkSurfaceFormatKHR GfRenderContext_Platform::SelectSwapchainFormat()
{
	if (m_tSupportedFormats.size() == 1 &&
		m_tSupportedFormats[0].format == VK_FORMAT_UNDEFINED) {
		return{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const VkSurfaceFormatKHR& format : m_tSupportedFormats)
	{
		if (format.format == VK_FORMAT_R8G8B8A8_UNORM) {
			return format;
		}
	}
	return m_tSupportedFormats[0];
}

////////////////////////////////////////////////////////////////////////////////

VkSurfaceTransformFlagBitsKHR GfRenderContext_Platform::SelectSwapchainTransform()
{
	if (m_pCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	return m_pCapabilities.currentTransform;
}

////////////////////////////////////////////////////////////////////////////////

VkImageUsageFlags GfRenderContext_Platform::SelectSwapchainFlags()
{
	if (m_pCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
		return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
			VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
	// Transfer DST usage is not permitted. DST usage is required for clear
	// operations
	return static_cast<VkImageUsageFlags>(-1);
}

////////////////////////////////////////////////////////////////////////////////

VkPresentModeKHR GfRenderContext_Platform::SelectSwapchainPresentMode()
{
	// Priorise MAILBOX mode (best option for games: When an image is generated it
	// replaces the image in the queue. So it is always going to show the latest
	// generated image)
	for (auto& mode : m_tSupportedPresentModes)
	{
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return mode;
		}
	}
	for (auto& mode : m_tSupportedPresentModes)
	{
		if (mode == VK_PRESENT_MODE_FIFO_KHR) {
			return mode;
		}
	}
	// Could not find a valid Present mode
	return static_cast<VkPresentModeKHR>(-1);
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::CreateInstance()
{
	// Check instance extensions
#if defined(_DEBUG) || defined (_DEBUGOPT)
	CheckInstanceAvalExtensions();
#endif // _DEBUG

	CheckValidationLayerSupport();

	VkApplicationInfo kAppInfo;
	kAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	kAppInfo.pNext = nullptr;
	kAppInfo.pApplicationName = ((GfRenderContext*)this)->m_pWindow->GetWindowName();
	kAppInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	kAppInfo.pEngineName = GF_ENGINE_NAME;
	kAppInfo.engineVersion = GF_ENGINE_VERSION;
	kAppInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo kInfo;
	kInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	kInfo.pNext = nullptr;
	kInfo.flags = 0;
	kInfo.pApplicationInfo = &kAppInfo;
	kInfo.enabledLayerCount = g_uiLayerCount;
	kInfo.ppEnabledLayerNames = g_pLayerNames;
	kInfo.enabledExtensionCount = g_uiInstanceExtensionCount;
	kInfo.ppEnabledExtensionNames = &g_pInstanceExtensions[0];

	// Create Vulkan instance
	VkResult eResult = vkCreateInstance(&kInfo, nullptr, &m_pInstance);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create VKInstance");
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::CreateSurface()
{
	GfWindow_Platform* pWindowPlat(((GfRenderContext*)this)->m_pWindow);

	VkWin32SurfaceCreateInfoKHR kSurfaceInfo;
	kSurfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	kSurfaceInfo.pNext = nullptr;
	kSurfaceInfo.flags = 0;
	kSurfaceInfo.hinstance = GetModuleHandle(NULL); //pWindowPlat->GetInstance();
	kSurfaceInfo.hwnd = pWindowPlat->GetHwnd();
	VkResult eResult = vkCreateWin32SurfaceKHR(m_pInstance, &kSurfaceInfo, nullptr, &m_pSurface);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create Win32 Surface");
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::CreateDevice()
{
	u32 uiDeviceCount = 0;
	VkResult eResult = vkEnumeratePhysicalDevices(m_pInstance, &uiDeviceCount, nullptr);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to get number of physical devices");
	GF_ASSERT(uiDeviceCount != 0, "Incorrect number of physical devices");

	{
		std::vector<VkPhysicalDevice> devices(uiDeviceCount);
		eResult = vkEnumeratePhysicalDevices(m_pInstance, &uiDeviceCount, &devices[0]);
		GF_ASSERT(eResult == VK_SUCCESS, "Failed to get physical devices");

		u32 uiPresentFamilyIdx(UINT32_MAX);
		u32 uiGraphicsFamilyIdx(UINT32_MAX);
		u32 uiTransferFamilyIdx(UINT32_MAX);
		u32 uiComputeFamilyIdx(UINT32_MAX);
		u32 uiAsyncComputeFamilyIdx(UINT32_MAX);

		// Search for valid family indices for the different queues
		for (u32 i = 0; i < uiDeviceCount; i++)
		{
			if (CheckPhysicalDeviceProperties(devices[i], uiGraphicsFamilyIdx, uiPresentFamilyIdx))
			{
				// This device supports graphics and using swap chains
				m_pPhysicalDevice = devices[i];
				break;
			}
		}

		// Assign the families indices
		m_kBase.m_pAvailableFamilies[GfRencerContextFamilies::Present] = uiPresentFamilyIdx;
		m_kBase.m_pAvailableFamilies[GfRencerContextFamilies::Graphics] = uiGraphicsFamilyIdx;
		m_kBase.m_pAvailableFamilies[GfRencerContextFamilies::Transfer] = uiGraphicsFamilyIdx;
		// It'll be the graphics queue
		m_kBase.m_pAvailableFamilies[GfRencerContextFamilies::Compute] = uiComputeFamilyIdx;
		// Temporarily async compute will fallback to the compute queue. In the end it will probably be the graphics queue
		m_kBase.m_pAvailableFamilies[GfRencerContextFamilies::AsyncCompute] = uiComputeFamilyIdx;

		// Check extensions
#if defined(_DEBUG) || defined (_DEBUGOPT)
		CheckDeviceAvalExtensions(m_pPhysicalDevice);
#endif // _DEBUG

		GF_ASSERT(m_pPhysicalDevice, "A valid physical device was not found");
		static constexpr u32 s_uiPrioritiesCount(1);
		f32 pQueuePriorities[s_uiPrioritiesCount] = { 1.0f };

		// Queue index 0
		VkDeviceQueueCreateInfo kQueueInfo;
		kQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		kQueueInfo.pNext = nullptr;
		kQueueInfo.flags = 0;
		kQueueInfo.queueFamilyIndex = uiGraphicsFamilyIdx;
		kQueueInfo.queueCount = s_uiPrioritiesCount;
		kQueueInfo.pQueuePriorities = pQueuePriorities;

		std::vector<VkDeviceQueueCreateInfo> tQueueInfoList;
		tQueueInfoList.push_back(kQueueInfo);

		if (m_kBase.GetFamilyIdx(GfRencerContextFamilies::Graphics) != m_kBase.GetFamilyIdx(GfRencerContextFamilies::Present)) {
			// Almost the same configuration apart of the family index to use
			kQueueInfo.queueFamilyIndex = uiPresentFamilyIdx;
			tQueueInfoList.push_back(kQueueInfo);
		}

		// Features to be enabled, by default it is all disabled
		VkPhysicalDeviceFeatures kFeatures{};

		VkDeviceCreateInfo kDeviceInfo;
		kDeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		kDeviceInfo.pNext = nullptr;
		kDeviceInfo.flags = 0;
		kDeviceInfo.queueCreateInfoCount = static_cast<u32>(tQueueInfoList.size());
		kDeviceInfo.pQueueCreateInfos = &tQueueInfoList[0];
		kDeviceInfo.enabledLayerCount = 0;
		kDeviceInfo.ppEnabledLayerNames = nullptr;
		kDeviceInfo.enabledExtensionCount = g_uiDeviceExtensionCount;
		kDeviceInfo.ppEnabledExtensionNames = &g_pDeviceExtensions[0];
		kDeviceInfo.pEnabledFeatures = &kFeatures;
		eResult = vkCreateDevice(m_pPhysicalDevice, &kDeviceInfo, nullptr, &m_pDevice);
		GF_ASSERT(eResult == VK_SUCCESS, "Failed to create logical device");
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::RetrieveQueues()
{
	// Get access to the queues we will use
	for (u32 i=0; i<GfRencerContextFamilies::Count; ++i) 
	{
		u32 uiFamilyIdx(m_kBase.GetFamilyIdx((GfRencerContextFamilies::Type)i));
		if (uiFamilyIdx != GfRencerContextFamilies::InvalidIdx) 
		{
			vkGetDeviceQueue(m_pDevice, uiFamilyIdx, 0, &m_pQueues[i]);	
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::CreateSwapchain()
{
	// Get the capabilities of the surface
	VkResult eResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_pPhysicalDevice, m_pSurface, &m_pCapabilities);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to get the capabilities of the surface");
	
	// Get the range of image formats allowed in the surface
	u32 uiFormatCount(0);
	eResult = vkGetPhysicalDeviceSurfaceFormatsKHR(m_pPhysicalDevice, m_pSurface, &uiFormatCount, nullptr);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to enumerate the formats of the surface");
	m_tSupportedFormats.resize(uiFormatCount);
	
	eResult = vkGetPhysicalDeviceSurfaceFormatsKHR(
		m_pPhysicalDevice, m_pSurface, &uiFormatCount, &m_tSupportedFormats[0]);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to get the formats of the surface");
	
	// Get the supported present modes
	u32 presentModeCount = 0;
	eResult = vkGetPhysicalDeviceSurfacePresentModesKHR(
		m_pPhysicalDevice, m_pSurface, &presentModeCount, nullptr);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to enumerate the present modes");
	m_tSupportedPresentModes.resize(presentModeCount);
	
	eResult = vkGetPhysicalDeviceSurfacePresentModesKHR(
		m_pPhysicalDevice, m_pSurface, &presentModeCount,
		&m_tSupportedPresentModes[0]);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to get the supported present modes");

	// Images extend
	VkExtent2D kExtend;
	kExtend.width = m_kBase.m_pWindow->GetWidth();
	kExtend.height = m_kBase.m_pWindow->GetHeight();

	// Get the number of buffers to create in the swap chain
	u32 bufferCount = GfClamp<u32>(GfRenderConstants::ms_uiNBufferingCount, m_pCapabilities.minImageCount, m_pCapabilities.maxImageCount);
	// Select a image format to use in the swap chain
	m_kSwapChainFormat = SelectSwapchainFormat();
	// Get the transform to apply to the swap chain (Useful in mobiles when using
	// Landscape or portrait)
	VkSurfaceTransformFlagBitsKHR transform = SelectSwapchainTransform();
	// Get the flags to use in the images of the swap chain
	VkImageUsageFlags usageFlags = SelectSwapchainFlags();
	// Get the Present mode to use
	VkPresentModeKHR presentMode = SelectSwapchainPresentMode();

	VkSwapchainKHR oldSwapChain = VK_NULL_HANDLE;
	VkSwapchainCreateInfoKHR swapChainInfo{};
	swapChainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainInfo.pNext = nullptr;
	swapChainInfo.flags = 0;
	swapChainInfo.surface = m_pSurface;
	swapChainInfo.minImageCount = bufferCount;
	swapChainInfo.imageFormat = m_kSwapChainFormat.format;
	swapChainInfo.imageColorSpace = m_kSwapChainFormat.colorSpace;
	swapChainInfo.imageExtent = kExtend;
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

	eResult = vkCreateSwapchainKHR(m_pDevice, &swapChainInfo, nullptr, &m_pSwapChain);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create swap chain");
	if (oldSwapChain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(m_pDevice, oldSwapChain, nullptr);
	}
	CheckSwapchainImages();
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::CreateSyncPrimitives()
{
	// Semaphore config
	VkSemaphoreCreateInfo kSemaphoreInfo{};
	kSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	kSemaphoreInfo.pNext = nullptr;
	kSemaphoreInfo.flags = 0;

	for (u32 i = 0; i < GfRenderConstants::ms_uiNBufferingCount; ++i) 
	{
		VkResult eResult = vkCreateSemaphore(m_pDevice, &kSemaphoreInfo, nullptr,
			&m_pFrameSyncEntries[i].m_pFinishedRendering);
		GF_ASSERT(eResult == VK_SUCCESS, "Failed to create semaphore");
		eResult = vkCreateSemaphore(m_pDevice, &kSemaphoreInfo, nullptr,
			&m_pFrameSyncEntries[i].m_pImageReady);
		GF_ASSERT(eResult == VK_SUCCESS, "Failed to create semaphore");
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF