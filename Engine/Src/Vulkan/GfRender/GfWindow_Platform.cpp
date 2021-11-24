////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/13
//	File: 	GFWINDOW_PLATFORM.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GfWindow.h"
#include "Common/GfInput/GfInput.h"
#include "Common/GfRender/GfRenderContext.h"

#include "Common/GfCore/GfMaths.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GfRender_Platform.h)

#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
// Functions

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
		// This message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	}
	break;
	}

	GfInput_Platform::HandleInput(message, wParam, lParam);

	return DefWindowProc(hWnd, message, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////

bool ProcessEvents(MSG* msg)
{
	// Check to see if any messages are waiting in the queue
	if (PeekMessage(msg, NULL, 0, 0, PM_REMOVE)) {
		// translate keystroke messages into the right format
		TranslateMessage(msg);
		// send the message to the WindowProc function
		DispatchMessage(msg);
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// Header impl

GF_DEFINE_PLATFORM_CTOR(GfWindow)
	, m_pHwnd(nullptr)
	, m_pInstance(nullptr)
	, m_pSurface(nullptr)
{}

////////////////////////////////////////////////////////////////////////////////

void GfWindow_Platform::initRHI(GfWindowInitParams& kInitParams)
{
	// Assign the HANDLE
	m_pInstance = GetModuleHandle(NULL);

	// this struct holds information for the window class
	WNDCLASSEX kWc;
	// clear out the window class for use
	ZeroMemory(&kWc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	kWc.cbSize = sizeof(WNDCLASSEX);
	kWc.style = CS_HREDRAW | CS_VREDRAW;
	kWc.lpfnWndProc = WindowProc;
	kWc.hInstance = m_pInstance;
	kWc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	kWc.hCursor = LoadCursor(NULL, IDC_ARROW);
	kWc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	kWc.lpszMenuName = NULL;
	kWc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	// Window name
	kWc.lpszClassName = kInitParams.m_szWindowName;

	// register the window class
	RegisterClassEx(&kWc);

	DEVMODE dev_screen_settings;
	s32 siPosX = 0, siPosY = 0;
	if (kInitParams.m_bFullScreen) {
		m_kBase.m_width = GetSystemMetrics(SM_CXSCREEN);
		m_kBase.m_height = GetSystemMetrics(SM_CYSCREEN);
		memset(&dev_screen_settings, 0, sizeof(dev_screen_settings));
		dev_screen_settings.dmSize = sizeof(dev_screen_settings);
		dev_screen_settings.dmPelsWidth = (unsigned long)m_kBase.m_width;
		dev_screen_settings.dmPelsHeight = (unsigned long)m_kBase.m_height;
		dev_screen_settings.dmBitsPerPel = 32;
		dev_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&dev_screen_settings, CDS_FULLSCREEN);
	}
	else
	{
		siPosX = GetSystemMetrics(SM_CXSCREEN) >> 2;
		siPosY = GetSystemMetrics(SM_CYSCREEN) >> 2;
	}

	// set the size, but not the position
	RECT wr = { 0, 0, (s64)m_kBase.m_width, (s64)m_kBase.m_height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); // adjust the size

	// create the window and use the result as the handle
	m_pHwnd = CreateWindowEx(NULL,
		kInitParams.m_szWindowName,						// name of the window class
		kInitParams.m_szWindowName,						// title of the window
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU,	// window style
		siPosX,											// x-position of the window
		siPosY,											// y-position of the window
		wr.right - wr.left,								// width of the window
		wr.bottom - wr.top,								// height of the window
		NULL,			   								// we have no parent window, NULL
		NULL,			   								// we aren't using menus, NULL
		GetModuleHandle(NULL),							// application handle
		NULL);											// used with multiple windows, NULL
	
	GF_ASSERT(m_pHwnd, "Failed to create the window");

	// display the window on the screen
	ShowWindow(m_pHwnd, SW_SHOWNORMAL);
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow_Platform::CreateSurface(const GfRenderContext& kCtx)
{
	VkWin32SurfaceCreateInfoKHR kSurfaceInfo;
	kSurfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	kSurfaceInfo.pNext = nullptr;
	kSurfaceInfo.flags = 0;
	kSurfaceInfo.hinstance = GetModuleHandle(NULL); //pWindowPlat->GetInstance();
	kSurfaceInfo.hwnd = m_pHwnd;
	VkResult eResult = vkCreateWin32SurfaceKHR(kCtx.Plat().m_pInstance, &kSurfaceInfo, nullptr, &m_pSurface);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create Win32 Surface");
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow_Platform::CreateSwapchain(const GfRenderContext& kCtx)
{
	// Get the capabilities of the surface
	VkResult eResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(kCtx.Plat().m_pPhysicalDevice, m_pSurface, &m_pCapabilities);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to get the capabilities of the surface");

	// Get the range of image formats allowed in the surface
	u32 uiFormatCount(0);
	eResult = vkGetPhysicalDeviceSurfaceFormatsKHR(kCtx.Plat().m_pPhysicalDevice, m_pSurface, &uiFormatCount, nullptr);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to enumerate the formats of the surface");
	m_tSupportedFormats.resize(uiFormatCount);

	eResult = vkGetPhysicalDeviceSurfaceFormatsKHR(
		kCtx.Plat().m_pPhysicalDevice, m_pSurface, &uiFormatCount, &m_tSupportedFormats[0]);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to get the formats of the surface");

	// Get the supported present modes
	u32 presentModeCount = 0;
	eResult = vkGetPhysicalDeviceSurfacePresentModesKHR(
		kCtx.Plat().m_pPhysicalDevice, m_pSurface, &presentModeCount, nullptr);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to enumerate the present modes");
	m_tSupportedPresentModes.resize(presentModeCount);

	eResult = vkGetPhysicalDeviceSurfacePresentModesKHR(
		kCtx.Plat().m_pPhysicalDevice, m_pSurface, &presentModeCount,
		&m_tSupportedPresentModes[0]);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to get the supported present modes");

	// Images extend
	VkExtent2D kExtend;
	kExtend.width = m_kBase.GetWidth();
	kExtend.height = m_kBase.GetHeight();

	// Get the number of buffers to create in the swap chain
	GF_ASSERT(m_pCapabilities.minImageCount >= GfRenderConstants::ms_uiNBufferingCount && 
		GfRenderConstants::ms_uiNBufferingCount <=m_pCapabilities.maxImageCount, "Invalid number of images");
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

	eResult = vkCreateSwapchainKHR(kCtx.Plat().m_pDevice, &swapChainInfo, nullptr, &m_pSwapChain);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create swap chain");
	if (oldSwapChain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(kCtx.Plat().m_pDevice, oldSwapChain, nullptr);
	}
	CheckSwapchainImages(kCtx);
}

////////////////////////////////////////////////////////////////////////////////

Bool GfWindow_Platform::TickRHI()
{
	MSG msg;
	// Check to see if any messages are waiting in the queue
	if (ProcessEvents(&msg))
	{
		// check to see if it's time to quit
		if (msg.message == WM_QUIT) 
		{
			return GF_FALSE;
		}
	}
	return GF_TRUE;
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow_Platform::ShutdownRHI()
{
	CloseWindow(m_pHwnd);
	m_pHwnd = nullptr;
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow_Platform::CheckSwapchainImages(const GfRenderContext& kCtx)
{
	u32 uiImageCount(0);
	VkResult eResult = vkGetSwapchainImagesKHR(kCtx.Plat().m_pDevice, m_pSwapChain, &uiImageCount, nullptr);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to enumerate the images of a swap chain");

	// Get the Images
	{
		m_tSwapChainImages.resize(uiImageCount);
		eResult = vkGetSwapchainImagesKHR(kCtx.Plat().m_pDevice, m_pSwapChain, &uiImageCount,
			&m_tSwapChainImages[0]);
		GF_ASSERT(eResult == VK_SUCCESS, "Failed to get the images of the swap chain");
	
		// Init base containers for back buffers
		m_kBase.m_tSwapchainTextures.clear();
		m_kBase.m_tSwapchainTextures.resize(uiImageCount);
		for(u32 i=0; i<uiImageCount; ++i)
		{
			SwapchainDesc kInit;
			kInit.m_pExternalImage = m_tSwapChainImages[i];
			kInit.m_format = ConvertTextureFormatToVkFormat(m_kSwapChainFormat.format);
			kInit.m_width = m_kBase.GetWidth();
			kInit.m_height = m_kBase.GetHeight();
			m_kBase.m_tSwapchainTextures[i].ExternalInit(kCtx, kInit);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow_Platform::CreateSyncPrimitives(const GfRenderContext& kCtx)
{
	// Semaphore config
	VkSemaphoreCreateInfo kSemaphoreInfo{};
	kSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	kSemaphoreInfo.pNext = nullptr;
	kSemaphoreInfo.flags = 0;

	for (u32 i = 0; i < GfRenderConstants::ms_uiNBufferingCount; ++i)
	{
		VkResult eResult = vkCreateSemaphore(kCtx.Plat().m_pDevice, &kSemaphoreInfo, nullptr,
			&m_pFrameSyncEntries[i].m_pFinishedRendering);
		GF_ASSERT(eResult == VK_SUCCESS, "Failed to create semaphore");
		eResult = vkCreateSemaphore(kCtx.Plat().m_pDevice, &kSemaphoreInfo, nullptr,
			&m_pFrameSyncEntries[i].m_pImageReady);
		GF_ASSERT(eResult == VK_SUCCESS, "Failed to create semaphore");
	}
}

////////////////////////////////////////////////////////////////////////////////

VkSurfaceFormatKHR GfWindow_Platform::SelectSwapchainFormat()
{
	if (m_tSupportedFormats.size() == 1 &&
		m_tSupportedFormats[0].format == VK_FORMAT_UNDEFINED) {
		return{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const VkSurfaceFormatKHR& format : m_tSupportedFormats)
	{
		if (format.format == VK_FORMAT_R8G8B8A8_UNORM) 
		{
			return format;
		}
	}
	return m_tSupportedFormats[0];
}

////////////////////////////////////////////////////////////////////////////////

VkSurfaceTransformFlagBitsKHR GfWindow_Platform::SelectSwapchainTransform()
{
	if (m_pCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	return m_pCapabilities.currentTransform;
}

////////////////////////////////////////////////////////////////////////////////

VkImageUsageFlags GfWindow_Platform::SelectSwapchainFlags()
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

VkPresentModeKHR GfWindow_Platform::SelectSwapchainPresentMode()
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

VkImageView GfWindow_Platform::GetCurrentBackBufferView() const
{
	return m_tSwapChainImageView[m_kBase.m_uiCurrentFrameIdx];
}

////////////////////////////////////////////////////////////////////////////////

VkImage GfWindow_Platform::GetCurrentBackBuffer() const
{
	return m_tSwapChainImages[m_kBase.m_uiCurrentFrameIdx];
}

////////////////////////////////////////////////////////////////////////////////

const GfFrameSyncing& GfWindow_Platform::GetFrameSyncPrimitives() const
{
	return m_pFrameSyncEntries[m_kBase.m_uiCurrentFrameIdx];
}

////////////////////////////////////////////////////////////////////////////////

VkSurfaceKHR GfWindow_Platform::GetSurface() const
{
	return m_pSurface;
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow_Platform::BeginFrameRHI(const GfRenderContext& kCtx)
{
	const GfFrameSyncing& kSyncPrimitives(GetFrameSyncPrimitives());
	VkResult eResult = vkAcquireNextImageKHR(kCtx.Plat().m_pDevice, m_pSwapChain, UINT64_MAX,
		kSyncPrimitives.m_pImageReady, VK_NULL_HANDLE,
		&m_kBase.m_uiCurrentFrameIdx);
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

void GfWindow_Platform::EndFrameRHI(const GfRenderContext& kCtx)
{
	const GfFrameSyncing& kSyncPrimitives(GetFrameSyncPrimitives());

	VkPresentInfoKHR kInfo;
	kInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	kInfo.pNext = nullptr;
	kInfo.waitSemaphoreCount = 1;
	kInfo.pWaitSemaphores = &kSyncPrimitives.m_pFinishedRendering;
	kInfo.swapchainCount = 1;
	kInfo.pSwapchains = &m_pSwapChain;
	kInfo.pImageIndices = &m_kBase.m_uiCurrentFrameIdx;
	kInfo.pResults = nullptr;
	auto result = vkQueuePresentKHR(kCtx.Plat().GetQueue(GfRenderContextFamilies::Graphics), &kInfo);
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

VkFormat GfWindow_Platform::GetSwapchainFormat() const
{
	return m_kSwapChainFormat.format;
}

////////////////////////////////////////////////////////////////////////////////
// EOF