////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfRenderSync_Platform.cpp
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GfRenderContext.h"
#include "Common/GfRender/GraphicResources/GfRenderSync.h"

////////////////////////////////////////////////////////////////////////////////


GF_DEFINE_PLATFORM_CTOR(GfSemaphore)
	, m_semaphore(VK_NULL_HANDLE)
{
}

void GfSemaphore_Platform::create(const GfRenderContext& ctx)
{
	VkSemaphoreCreateInfo kSemaphoreInfo{};
	kSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	kSemaphoreInfo.pNext = nullptr;
	kSemaphoreInfo.flags = 0;

	VkResult result = vkCreateSemaphore(ctx.Plat().m_pDevice, &kSemaphoreInfo, nullptr,	&m_semaphore);
	GF_ASSERT(result == VK_SUCCESS, "Wops!");
}

void GfSemaphore_Platform::destroy(const GfRenderContext& ctx)
{
	if (m_semaphore != VK_NULL_HANDLE) 
	{
		vkDestroySemaphore(ctx.Plat().m_pDevice, m_semaphore, nullptr);
		m_semaphore = VK_NULL_HANDLE;
	}
}
