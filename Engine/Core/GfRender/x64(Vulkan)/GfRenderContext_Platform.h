////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/15
//	File: 	GfRenderContext_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFRENDERCONTEXT_PLATFORM_H__
#define __GFRENDERCONTEXT_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfGraphicsSDK.h"
#include "GfRender/Common/GfRenderConstants.h"
#include <vector>

////////////////////////////////////////////////////////////////////////////////

struct GfFrameSyncing 
{
	VkSemaphore m_pFinishedRendering;
	VkSemaphore	m_pImageReady;
};

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext_Platform
{
public:

	friend class GfRenderContext;

	GfRenderContext_Platform(GfRenderContext& kBase);

	////////////////////////////////////////////////////////////////////////////////

	// Used as the final target of the frame
	VkImageView GetCurrentBackBufferView() const;

	VkImageView GetBackBufferView(u32 uiFrameIdx) const;

	VkImage GetCurrentBackBuffer() const;

	const GfFrameSyncing& GetFrameSyncPrimitives() const;

	VkQueue GetQueue(GfRencerContextFamilies::Type eType) const;

	VkFormat GetSwapchainFormat() const;

	// Main settings
	VkInstance						m_pInstance;
	VkPhysicalDevice				m_pPhysicalDevice;
	VkDevice						m_pDevice;

	////////////////////////////////////////////////////////////////////////////////

protected:

	void InitRHI();

	void BeginFrameRHI();

	void EndFrameRHI();

private:

	////////////////////////////////////////////////////////////////////////////////

	bool CheckPhysicalDeviceProperties(
		VkPhysicalDevice pDevice,
		u32& uiOutSelectedFamilyIndex,
		u32& uiOutSwapChainFamilyIndex);

	void CheckValidationLayerSupport();

	void CheckInstanceAvalExtensions();

	void CheckDeviceAvalExtensions(VkPhysicalDevice pPhysicalDevice);

	void CheckSwapchainImages();

	VkSurfaceFormatKHR SelectSwapchainFormat();

	VkSurfaceTransformFlagBitsKHR SelectSwapchainTransform();

	VkImageUsageFlags SelectSwapchainFlags();

	VkPresentModeKHR SelectSwapchainPresentMode();

	// Initialization steps

	void CreateInstance();

	void CreateSurface();
	
	void CreateDevice();

	void RetrieveQueues();

	void CreateSwapchain();

	void CreateSyncPrimitives();

	////////////////////////////////////////////////////////////////////////////////

	GfRenderContext&				m_kBase;

	// Queues
	VkQueue							m_pQueues[GfRencerContextFamilies::Count];

	// Swap chain and surface
	VkSurfaceKHR					m_pSurface;
	VkSwapchainKHR					m_pSwapChain;
	VkSurfaceFormatKHR				m_kSwapChainFormat;
	std::vector<VkImage>			m_tSwapChainImages;
	std::vector<VkImageView>		m_tSwapChainImageView;
	u32								m_uiCurrentImageIdx;

	// Settings
	VkSurfaceCapabilitiesKHR		m_pCapabilities;
	std::vector<VkSurfaceFormatKHR> m_tSupportedFormats;
	std::vector<VkPresentModeKHR>	m_tSupportedPresentModes;

	// Sync
	GfFrameSyncing					m_pFrameSyncEntries[GfRenderConstants::ms_uiNBufferingCount];
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkQueue GfRenderContext_Platform::GetQueue(GfRencerContextFamilies::Type eType) const
{
	GF_ASSERT(eType >= 0 && eType < GfRencerContextFamilies::Count, "Invalid family");
	return m_pQueues[eType];
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkImageView GfRenderContext_Platform::GetBackBufferView(u32 uiFrameIdx) const
{
	return m_tSwapChainImageView[uiFrameIdx];
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERCONTEXT_PLATFORM_H__