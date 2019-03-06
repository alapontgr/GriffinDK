////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/18
//	File: 	GfTexture2D_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFTEXTURE2D_PLATFORM_H__
#define __GFTEXTURE2D_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfRender/Common/GfGraphicsSDK.h"

#include "vma/vk_mem_alloc.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;
class GfTexture2D;
class GfCmdBuffer;
class GfBuffer;

////////////////////////////////////////////////////////////////////////////////

class GfTexture2D_Platform 
{
public:

	friend class GfTexture2D;

	GfTexture2D_Platform(GfTexture2D& kBase);

	bool CreateRHI(const GfRenderContext& kCtx);

	void DestroyRHI(const GfRenderContext& kCtx);

	VkImage GetImage() const;

	VkImageView GetView() const;

	VkImageAspectFlags GetAspectMask() const;

private:

	bool CreateImageRHI(const GfRenderContext &kCtx);

	bool CreateImageViewRHI(const GfRenderContext &kCtx);

	////////////////////////////////////////////////////////////////////////////////

	void LoadTexture2DDataFromStagingBufferRHI(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiBufferOffset);

	////////////////////////////////////////////////////////////////////////////////

	GfTexture2D& m_kBase;

	VkImage				m_pImage;
	VkImageView			m_pImageView;
	VkImageAspectFlags	m_uiAspectMask;

	// Vma Resources
	VmaAllocation		m_pAlloc;
	VmaAllocationInfo	m_kAllocInfo;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkImage GfTexture2D_Platform::GetImage() const
{
	return m_pImage;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkImageView GfTexture2D_Platform::GetView() const
{
	return m_pImageView;
}


////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkImageAspectFlags GfTexture2D_Platform::GetAspectMask() const
{
	return m_uiAspectMask;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTURE2D_PLATFORM_H__