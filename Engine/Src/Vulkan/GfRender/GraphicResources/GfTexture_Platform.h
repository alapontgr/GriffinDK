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

#include "Common/GfRender/GfGraphicsSDK.h"

#include "vma/vk_mem_alloc.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;
class GfTexture2D;
class GfCmdBuffer;
class GfBuffer;
struct TextureDesc;

////////////////////////////////////////////////////////////////////////////////

struct GfExternTexInit_Platform
{
	VkImage	m_pExternalImage;
};

////////////////////////////////////////////////////////////////////////////////

class GfTexture_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfTexture);
public:

	friend class GfTexture;

	void init(const TextureDesc& desc);

	void ExternalInitPlat(const GfExternTexInit_Platform& initParams);

	bool createRHI(const GfRenderContext& ctx);

	void destroyRHI(const GfRenderContext& ctx);

	bool allocateImageMemoryRHI(const GfRenderContext &ctx, VmaAllocationCreateInfo* allocInfo);

	bool bindImageWithMemoryRHI(const GfRenderContext &ctx);

	////////////////////////////////////////////////////////////////////////////////

	VkImage getImage() const;

	VkImageView getView() const;

	VkImageAspectFlags getAspectMask() const;

	VkImageLayout getCurrentLayout() const;

	void setCurrentLayout(VkImageLayout layout);

	void loadTexture2DDataFromStagingBufferRHI(const GfRenderContext& ctx, const GfCmdBuffer& cmdBuffer, const GfBuffer& from, u32 bufferOffset);

protected:

private:

	void createView(const GfRenderContext& kCtx);

	VkImageLayout		m_currLayout;
	VkImage				m_image;
	VkImageView			m_view; // Used for RenderTarget textures
	VkImageAspectFlags	m_uiAspectMask;

	// Vma Resources
	VmaAllocation		m_pAlloc;
	VmaAllocationInfo	m_kAllocInfo;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkImage GfTexture_Platform::getImage() const
{
	return m_image;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkImageAspectFlags GfTexture_Platform::getAspectMask() const
{
	return m_uiAspectMask;
}

GF_FORCEINLINE VkImageLayout GfTexture_Platform::getCurrentLayout() const
{
	return m_currLayout;
}

GF_FORCEINLINE VkImageView GfTexture_Platform::getView() const
{
	return m_view;
}

GF_FORCEINLINE void GfTexture_Platform::setCurrentLayout(VkImageLayout layout)
{
	m_currLayout = layout;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTURE_PLATFORM_H__