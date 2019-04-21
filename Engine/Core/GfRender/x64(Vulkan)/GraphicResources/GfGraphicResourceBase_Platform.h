////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/03/31
//	File: 	GfGraphicResourceBase_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFGRAPHICRESOURCEBASE_PLATFORM_H__
#define __GFGRAPHICRESOURCEBASE_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfRender/Common/GfGraphicsSDK.h"

#include "vma/vk_mem_alloc.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

////////////////////////////////////////////////////////////////////////////////

class GfTexturedResource_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfTexturedResource);
public:

	friend class GfTexturedResource;

	GfTexturedResource_Platform();

	bool CreateImageRHI(const GfRenderContext &kCtx, VkImageCreateInfo* pTextureInfo);

	bool AllocateImageMemoryRHI(const GfRenderContext &kCtx,VmaAllocationCreateInfo* pAllocInfo);

	bool BindImageWithMemoryRHI(const GfRenderContext &kCtx);

	void DestroyRHI(const GfRenderContext &kCtx);

	////////////////////////////////////////////////////////////////////////////////

	VkImage GetImage() const;

	VkImageAspectFlags GetAspectMask() const;

private:

	VkImage				m_pImage;
	VkImageAspectFlags	m_uiAspectMask;

	// Vma Resources
	VmaAllocation		m_pAlloc;
	VmaAllocationInfo	m_kAllocInfo;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkImage GfTexturedResource_Platform::GetImage() const
{
	return m_pImage;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkImageAspectFlags GfTexturedResource_Platform::GetAspectMask() const
{
	return m_uiAspectMask;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFGRAPHICRESOURCEBASE_PLATFORM_H__