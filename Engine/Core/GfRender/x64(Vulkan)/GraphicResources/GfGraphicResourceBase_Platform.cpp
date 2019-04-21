////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/03/31
//	File: 	GfGraphicResourceBase_Platform.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GraphicResources/GfGraphicResourceBase.h"
#include "GfRender/Common/GfRenderContext.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfTexturedResource)
	, m_pImage(VK_NULL_HANDLE)
	, m_uiAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
	, m_pAlloc(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexturedResource_Platform::CreateImageRHI(const GfRenderContext &kCtx, VkImageCreateInfo* pTextureInfo)
{
	if (vkCreateImage(kCtx.Plat().m_pDevice, pTextureInfo, nullptr, &m_pImage) != VK_SUCCESS)
	{
		return false;
	}
	// Assign aspect mask
	m_uiAspectMask = 0;
	if (m_kBase.IsDepthBuffer())
	{
		m_uiAspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	if (m_kBase.IsStencilBuffer())
	{
		m_uiAspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	if (!m_uiAspectMask)
	{
		m_uiAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexturedResource_Platform::AllocateImageMemoryRHI(const GfRenderContext &kCtx, VmaAllocationCreateInfo* pAllocInfo)
{
	if (vmaAllocateMemoryForImage(kCtx.Plat().m_kAllocator, m_pImage, pAllocInfo, &m_pAlloc, &m_kAllocInfo) != VK_SUCCESS)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexturedResource_Platform::BindImageWithMemoryRHI(const GfRenderContext &kCtx)
{
	if (vmaBindImageMemory(kCtx.Plat().m_kAllocator, m_pAlloc, m_pImage) != VK_SUCCESS)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void GfTexturedResource_Platform::DestroyRHI(const GfRenderContext &kCtx)
{
	if (m_pImage)
	{
		vkDestroyImage(kCtx.Plat().m_pDevice, m_pImage, nullptr);
		m_pImage = nullptr;

	}
	// Release allocated GPU memory
	if (m_pAlloc)
	{
		vmaFreeMemory(kCtx.Plat().m_kAllocator, m_pAlloc);
		m_pAlloc = nullptr;
		// Not really needed
		memset(&m_kAllocInfo, 0, sizeof(VmaAllocationInfo));
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF