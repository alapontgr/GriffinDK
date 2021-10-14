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

#include "Common/GfRender/GfGraphicsSDK.h"
#include "Common/GfRender/GfRenderConstants.h"
#include "Common/GfCore/GfStl.h"

#include "vma/vk_mem_alloc.h"

////////////////////////////////////////////////////////////////////////////////

class GfWindow;
class GfDescriptorSetFactoryVK;

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfRenderContext);
public:

	////////////////////////////////////////////////////////////////////////////////

	VkQueue GetQueue(GfRenderContextFamilies::Type eType) const;	

	GfDescriptorSetFactoryVK* getDescSetFactory() const { return m_descSetFactory; }

	// Main settings
	VkInstance						m_pInstance;
	VkPhysicalDevice				m_pPhysicalDevice;
	VkDevice						m_pDevice;

	////////////////////////////////////////////////////////////////////////////////
	// GPU memory

	VmaAllocator					m_kAllocator;

	////////////////////////////////////////////////////////////////////////////////

protected:

	void PreInitRHI(GfWindow* pWindow);

	void initRHI(GfWindow* pWindow);	

	void shutdown();

	void tick();

private:

	////////////////////////////////////////////////////////////////////////////////

	bool CheckPhysicalDeviceProperties(
		VkPhysicalDevice pDevice,
		VkSurfaceKHR pSurface,
		u32& uiOutSelectedFamilyIndex,
		u32& uiOutSwapChainFamilyIndex);

	void CheckValidationLayerSupport();

	void CheckInstanceAvalExtensions();

	void CheckDeviceAvalExtensions(VkPhysicalDevice pPhysicalDevice);

	// Initialization steps

	void CreateInstance(GfWindow* pWindow);
	
	void CreateDevice(GfWindow* pWindow);

	void RetrieveQueues();

	void CreateVulkanAllocator();

	////////////////////////////////////////////////////////////////////////////////
	// Queues
	VkQueue	m_pQueues[GfRenderContextFamilies::Count];

	GfDescriptorSetFactoryVK* m_descSetFactory;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkQueue GfRenderContext_Platform::GetQueue(GfRenderContextFamilies::Type eType) const
{
	GF_ASSERT(eType >= 0 && eType < GfRenderContextFamilies::Count, "Invalid family");
	return m_pQueues[eType];
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERCONTEXT_PLATFORM_H__