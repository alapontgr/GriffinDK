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

struct GfWindowInitParams;

class GfWindow_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfWindow);
public:

	void beginFrameRHI(const GfRenderContext& kCtx);

	void endFrameRHI(const GfRenderContext& kCtx);

	////////////////////////////////////////////////////////////////////////////////

	HWND getHwnd();

	HINSTANCE getInstance();

	VkFormat getSwapchainFormat() const;

	// Used as the final target of the frame
	VkImageView getCurrentBackBufferView() const;

	VkImageView getBackBufferView(u32 uiFrameIdx) const;

	VkImage getCurrentBackBuffer() const;

	VkSurfaceKHR getSurface() const;

private:

	void initRHI(GfWindowInitParams& kInitParams);

	void createSurface(const GfRenderContext& kCtx);

	void createSwapchain(const GfRenderContext& kCtx);

	Bool tickRHI();

	void shutdownRHI();

	////////////////////////////////////////////////////////////////////////////////

	void checkSwapchainImages(const GfRenderContext& kCtx);

	VkSurfaceFormatKHR selectSwapchainFormat();

	VkSurfaceTransformFlagBitsKHR selectSwapchainTransform();

	VkImageUsageFlags selectSwapchainFlags();

	VkPresentModeKHR selectSwapchainPresentMode();

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

};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE HWND GfWindow_Platform::getHwnd() 
{
	return m_pHwnd;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE HINSTANCE GfWindow_Platform::getInstance()
{
	return m_pInstance;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkImageView GfWindow_Platform::getBackBufferView(u32 uiFrameIdx) const
{
	return m_tSwapChainImageView[uiFrameIdx];
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFWINDOW_PLATFORM_H__