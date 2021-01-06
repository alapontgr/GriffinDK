////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/13
//	File: 	GfWindow_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFWINDOW_PLATFORM_H__
#define __GFWINDOW_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfStl.h"
#include "Common/GfRender/GfGraphicsSDK.h"
#include "Common/GfRender/GfWindow.h"
#include "Common/GfRender/GfRenderConstants.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

////////////////////////////////////////////////////////////////////////////////

struct GfWindowInitParams_Platform
{

};

////////////////////////////////////////////////////////////////////////////////

struct GfFrameSyncing
{
	VkSemaphore m_pFinishedRendering;
	VkSemaphore	m_pImageReady;
};

////////////////////////////////////////////////////////////////////////////////

struct GfWindowInitParams;

class GfWindow_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfWindow);
public:

	void BeginFrameRHI(const GfRenderContext& kCtx);

	void EndFrameRHI(const GfRenderContext& kCtx);

	////////////////////////////////////////////////////////////////////////////////

	HWND GetHwnd();

	HINSTANCE GetInstance();

	VkFormat GetSwapchainFormat() const;

	// Used as the final target of the frame
	VkImageView GetCurrentBackBufferView() const;

	VkImageView GetBackBufferView(u32 uiFrameIdx) const;

	VkImage GetCurrentBackBuffer() const;

	const GfFrameSyncing& GetFrameSyncPrimitives() const;

	VkSurfaceKHR GetSurface() const;

private:

	void InitRHI(GfWindowInitParams& kInitParams);

	void CreateSurface(const GfRenderContext& kCtx);

	void CreateSwapchain(const GfRenderContext& kCtx);

	void CreateSyncPrimitives(const GfRenderContext& kCtx);

	Bool TickRHI();

	void ShutdownRHI();

	////////////////////////////////////////////////////////////////////////////////

	void CheckSwapchainImages(const GfRenderContext& kCtx);

	VkSurfaceFormatKHR SelectSwapchainFormat();

	VkSurfaceTransformFlagBitsKHR SelectSwapchainTransform();

	VkImageUsageFlags SelectSwapchainFlags();

	VkPresentModeKHR SelectSwapchainPresentMode();

	////////////////////////////////////////////////////////////////////////////////

	HWND			m_pHwnd;
	HINSTANCE		m_pInstance;
	VkSurfaceKHR	m_pSurface;

	// Swap chain and surface
	VkSwapchainKHR					m_pSwapChain;
	VkSurfaceFormatKHR				m_kSwapChainFormat;
	GfVector<VkImage>				m_tSwapChainImages;
	GfVector<VkImageView>			m_tSwapChainImageView;

	// Settings
	VkSurfaceCapabilitiesKHR		m_pCapabilities;
	GfVector<VkSurfaceFormatKHR>	m_tSupportedFormats;
	GfVector<VkPresentModeKHR>		m_tSupportedPresentModes;

	// Sync
	GfFrameSyncing					m_pFrameSyncEntries[GfRenderConstants::ms_uiNBufferingCount];
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE HWND GfWindow_Platform::GetHwnd() 
{
	return m_pHwnd;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE HINSTANCE GfWindow_Platform::GetInstance()
{
	return m_pInstance;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkImageView GfWindow_Platform::GetBackBufferView(u32 uiFrameIdx) const
{
	return m_tSwapChainImageView[uiFrameIdx];
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFWINDOW_PLATFORM_H__