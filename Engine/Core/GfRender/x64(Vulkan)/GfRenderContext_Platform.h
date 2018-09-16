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
#include <vector>

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext_Platform
{
public:

	friend class GfRenderContext;

	GfRenderContext_Platform();

	////////////////////////////////////////////////////////////////////////////////

	// Main settings
	VkInstance						m_pInstance;
	VkPhysicalDevice				m_pPhysicalDevice;
	VkDevice						m_pDevice;

	// Used Families
	u32								m_uiGraphicsFamilyIndex;
	u32								m_uiPresentFamilyIndex;

	////////////////////////////////////////////////////////////////////////////////

private:

	void InitInternal();

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

	// Queues
	VkQueue							m_uiGraphicsQueue;
	VkQueue							m_uiPresentQueue;

	// Swap chain and surface
	VkSurfaceKHR					m_pSurface;
	VkSwapchainKHR					m_pSwapChain;
	VkSurfaceFormatKHR				m_pSwapChainFormat;
	std::vector<VkImage>			m_tSwapChainImages;
	std::vector<VkImageView>		m_tSwapChainImageView;

	// Settings
	VkSurfaceCapabilitiesKHR		m_pCapabilities;
	std::vector<VkSurfaceFormatKHR> m_tSupportedFormats;
	std::vector<VkPresentModeKHR>	m_tSupportedPresentModes;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERCONTEXT_PLATFORM_H__