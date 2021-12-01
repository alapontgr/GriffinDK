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

	void externalInitPlat(const GfRenderContext& ctx, const GfExternTexInit_Platform& initParams);

	bool create(const GfRenderContext& ctx);

	void destroy(const GfRenderContext& ctx);

	bool allocateImageMemoryRHI(const GfRenderContext &ctx, VmaAllocationCreateInfo* allocInfo);

	bool bindImageWithMemoryRHI(const GfRenderContext &ctx);

	////////////////////////////////////////////////////////////////////////////////

	VkImage getImage() const;

	VkImageView getDefaultView() const;

	u64 getDefaultViewID() const;

	u64 getViewIDForConfig(const GfRenderContext& ctx, const struct GfTextureViewConfig& config);

	VkImageAspectFlags getAspectMask() const;

	void loadTexture2DDataFromStagingBufferRHI(const GfRenderContext& ctx, const GfCmdBuffer& cmdBuffer, const GfBuffer& from, u32 bufferOffset);

protected:

private:

	void createView(const GfRenderContext& kCtx);

	VkImageLayout		m_currLayout;
	VkImage				m_image;
	VkImageAspectFlags	m_uiAspectMask;
	
	VkImageView			m_defaultView; // Used for RenderTarget textures
	GfUMap<u64, VkImageView> m_views;

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

GF_FORCEINLINE VkImageView GfTexture_Platform::getDefaultView() const
{
	return m_defaultView;
}

static_assert(sizeof(u64) == sizeof(VkImageView), "Wops!");
GF_FORCEINLINE u64 GfTexture_Platform::getDefaultViewID() const
{
	return reinterpret_cast<u64>(m_defaultView);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTURE_PLATFORM_H__