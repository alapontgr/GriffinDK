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

////////////////////////////////////////////////////////////////////////////////

class GfTexture2D_Platform 
{
public:

	friend class GfTexture2D;

	GfTexture2D_Platform(GfTexture2D& kBase);

	bool CreateRHI(const GfRenderContext& kCtx);

	void DestroyRHI(const GfRenderContext& kCtx);

private:

	bool CreateImageRHI(const GfRenderContext &kCtx);

	bool CreateImageViewRHI(const GfRenderContext &kCtx);

	GfTexture2D& m_kBase;

	VkImage		m_pImage;
	VkImageView m_pImageView;

	// Vma Resources
	VmaAllocation		m_pAlloc;
	VmaAllocationInfo	m_kAllocInfo;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTURE2D_PLATFORM_H__