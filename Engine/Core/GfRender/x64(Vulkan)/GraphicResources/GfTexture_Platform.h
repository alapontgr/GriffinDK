////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/18
//	File: 	GfTexture_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFTEXTURE_PLATFORM_H__
#define __GFTEXTURE_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfRender/Common/GfGraphicsSDK.h"

#include "vma/vk_mem_alloc.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;
class GfTexture2D;
class GfCmdBuffer;
class GfBuffer;

////////////////////////////////////////////////////////////////////////////////

struct GfExternTexInit_Platform
{
	VkImage	m_pExternalImage;
};

////////////////////////////////////////////////////////////////////////////////

class GfTexturedResource_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfTexturedResource);
public:

	friend class GfTexturedResource;

	GfTexturedResource_Platform();

	void ExternalInitPlat(const GfExternTexInit_Platform& kInitParams);

	bool CreateImageRHI(const GfRenderContext &kCtx, VkImageCreateInfo* pTextureInfo);

	bool AllocateImageMemoryRHI(const GfRenderContext &kCtx, VmaAllocationCreateInfo* pAllocInfo);

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

class GfTexture2D_Platform 
{
	GF_DECLARE_PLATFORM_MEMBERS(GfTexture2D);
public:

	bool CreateRHI(const GfRenderContext& kCtx);

	void DestroyRHI(const GfRenderContext& kCtx);

private:

	bool CreateImageRHI(const GfRenderContext &kCtx);

	////////////////////////////////////////////////////////////////////////////////

	void LoadTexture2DDataFromStagingBufferRHI(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiBufferOffset);
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
#endif // __GFTEXTURE_PLATFORM_H__