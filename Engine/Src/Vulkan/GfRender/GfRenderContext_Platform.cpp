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

#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"

#include "Common/GfRender/GfRenderContext.h"
#include "Common/GfRender/GfWindow.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GfRender_Platform.h)
#include GF_SOLVE_GFX_API_PATH(GfRender/GfShaderPipeline_Platform.h)

#include "Common/GfCore/GfMaths.h"
#include "Common/GfCore/GfStl.h"

#include <algorithm>

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
	"VK_LAYER_KHRONOS_validation" 
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

GF_DEFINE_PLATFORM_CTOR(GfRenderContext)
	, m_pInstance(nullptr)
	, m_pPhysicalDevice(nullptr)
	, m_pDevice(nullptr)
{
	for (u32 i=0; i<GfRenderContextFamilies::Count; ++i) 
	{
		m_pQueues[i] = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::PreInitRHI(GfWindow* pWindow)
{
	CreateInstance(pWindow);
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::initRHI(GfWindow* pWindow)
{
	CreateDevice(pWindow);
	RetrieveQueues();
	CreateVulkanAllocator();
	// Factories
	m_descSetFactory = new GfDescriptorSetFactoryVK(m_pDevice);
}

void GfRenderContext_Platform::shutdown()
{
	m_descSetFactory->shutdown();

	// Shutdown device
	GF_ASSERT_ALWAYS("TODO: Implement proper shutdown logic");

	// Finally release memory
	delete m_descSetFactory;
}

void GfRenderContext_Platform::tick() 
{

}

////////////////////////////////////////////////////////////////////////////////

bool GfRenderContext_Platform::CheckPhysicalDeviceProperties(
	VkPhysicalDevice pDevice,
	VkSurfaceKHR pSurface,
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
		GfVector<VkBool32> tSwapChainSupport(uiFamilyQueuesCount);
		GfVector<VkQueueFamilyProperties> tFamilies(uiFamilyQueuesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &uiFamilyQueuesCount, &tFamilies[0]);
		for (u32 i = 0; i < uiFamilyQueuesCount; i++)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(pDevice, i, pSurface, &tSwapChainSupport[i]);
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
		if (uiOutGraphicsFamilyIndex == UINT32_MAX ||
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
#if !GF_USE_VALIDATION_LAYERS
	return;
#endif
	u32 uiLayerCount;
	vkEnumerateInstanceLayerProperties(&uiLayerCount, nullptr);

	GfVector<VkLayerProperties> tAvailableLayers(uiLayerCount);
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
		GfVector<VkExtensionProperties> tProperties(uiExtensionCount);
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

void GfRenderContext_Platform::CreateInstance(GfWindow* pWindow)
{
	// Check instance extensions
#if defined(_DEBUG) || defined (_DEBUGOPT)
	CheckInstanceAvalExtensions();
#endif // _DEBUG

	CheckValidationLayerSupport();

	VkApplicationInfo kAppInfo;
	kAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	kAppInfo.pNext = nullptr;
	kAppInfo.pApplicationName = pWindow->getWindowName();
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

void GfRenderContext_Platform::CreateDevice(GfWindow* pWindow)
{
	u32 uiDeviceCount = 0;
	VkResult eResult = vkEnumeratePhysicalDevices(m_pInstance, &uiDeviceCount, nullptr);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to get number of physical devices");
	GF_ASSERT(uiDeviceCount != 0, "Incorrect number of physical devices");

	{
		GfVector<VkPhysicalDevice> devices(uiDeviceCount);
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
			if (CheckPhysicalDeviceProperties(devices[i], pWindow->Plat().getSurface(), uiGraphicsFamilyIdx, uiPresentFamilyIdx))
			{
				// This device supports graphics and using swap chains
				m_pPhysicalDevice = devices[i];
				break;
			}
		}

		// Assign the families indices
		m_kBase.m_pAvailableFamilies[GfRenderContextFamilies::Graphics] = uiGraphicsFamilyIdx;
		m_kBase.m_pAvailableFamilies[GfRenderContextFamilies::Transfer] = uiGraphicsFamilyIdx;
		// It'll be the graphics queue
		m_kBase.m_pAvailableFamilies[GfRenderContextFamilies::Compute] = uiComputeFamilyIdx;

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

		GfVector<VkDeviceQueueCreateInfo> tQueueInfoList;
		tQueueInfoList.push_back(kQueueInfo);

		//if (m_kBase.GetFamilyIdx(GfRenderContextFamilies::Graphics) != m_kBase.GetFamilyIdx(GfRenderContextFamilies::Present)) {
		//	// Almost the same configuration apart of the family index to use
		//	kQueueInfo.queueFamilyIndex = uiPresentFamilyIdx;
		//	tQueueInfoList.push_back(kQueueInfo);
		//}

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
	for (u32 i=0; i<GfRenderContextFamilies::Count; ++i) 
	{
		u32 uiFamilyIdx(m_kBase.GetFamilyIdx((GfRenderContextFamilies::Type)i));
		if (uiFamilyIdx != GfRenderContextFamilies::InvalidIdx) 
		{
			vkGetDeviceQueue(m_pDevice, uiFamilyIdx, 0, &m_pQueues[i]);	
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext_Platform::CreateVulkanAllocator()
{
	VmaAllocatorCreateInfo kAllocatorInfo = {};
	kAllocatorInfo.physicalDevice = m_pPhysicalDevice;
	kAllocatorInfo.device = m_pDevice;
	kAllocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
	kAllocatorInfo.instance = m_pInstance;
	vmaCreateAllocator(&kAllocatorInfo, &m_kAllocator);
}

////////////////////////////////////////////////////////////////////////////////
// EOF