////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/18
//	File: 	GfTexture.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFTEXTURE_H__
#define __GFTEXTURE_H__
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfRender/GfRenderCommon.h"
#include "Common/GfRender/GraphicResources/GfGraphicResourcesShared.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GraphicResources/GfTexture_Platform.h)

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

////////////////////////////////////////////////////////////////////////////////

enum class TextureType : u32 
{
	Type_1D = 0,
	Type_2D,
	Type_3D,
	COUNT
};

enum class TextureNumSamples : u32 
{
	None = 0,
	x4,
	x8,
	x16
};

struct TextureDesc
{
	u32						m_width			= 0;
	u32						m_height		= 0;
	u32						m_depth			= 1;
	u32						m_slices		= 1;
	u32						m_mipCount		= 1;
	TextureNumSamples		m_numSamples	= TextureNumSamples::None;
	TextureType				m_textureType	= TextureType::Type_2D;
	GfTextureFormat::Type	m_format		= GfTextureFormat::R8G8B8A8_UNorm;
	TextureUsageFlags::Mask	m_usage			= TextureUsageFlags::SRVAll;
	bool					m_mappable		= false;
};

struct GfTextureViewConfig 
{
	GfTextureViewConfig()
		: m_firstMipLevel(0)
		, m_mipLevelCount(0xffff)
		, m_firstSlice(0)
		, m_sliceCount(0xffff)
	{}

	u64 getHash() const { return *reinterpret_cast<const u64*>(this);  }

	u32 m_firstMipLevel : 16;
	u32 m_mipLevelCount : 16;
	u32 m_firstSlice : 16;
	u32 m_sliceCount : 16;
};
using GfTextureViewID = u64;

// Used to configure what areas of the tetxure to view
class GfTextureView
{
public:
	GfTextureView(class GfTexture* texture, const GfTextureViewConfig& config);
	GfTextureView();

	u64 getViewID(const GfRenderContext& ctx);

	GfTexture* getTexture() const { return m_texture; }

	const GfTextureViewConfig& getConfig() const { return m_config; }

private:

	GfTextureViewConfig m_config;
	class GfTexture* m_texture;
};

// Used to initialize swapchain iomages internally
struct SwapchainDesc : public GfExternTexInit_Platform 
{
	u32						m_width;
	u32						m_height;
	GfTextureFormat::Type		m_format = GfTextureFormat::R8G8B8A8_UNorm;
};

////////////////////////////////////////////////////////////////////////////////

// Base class for texture types
class GfTexture
{
	GF_DECLARE_PLATFORM_INTERFACE(GfTexture);
public:

	enum Flags : u32 
	{
		Initialized = 1<<0,
		InitializedAsSwapchain = 1<<1 | Initialized,
		GPUReady = 1<<2,
	};

	GfTexture();

	bool init(const TextureDesc& desc);

	void Create(const GfRenderContext& kCtx);

	void Destroy(const GfRenderContext& kCtx);

	// Init the Texture view with externally created resources (i.e. backbuffer)
	void ExternalInit(const GfRenderContext& ctx, const SwapchainDesc& kInitParams);

	GfTextureViewID getViewIDForConfig(const GfRenderContext& ctx, const struct GfTextureViewConfig& config);

	bool getIsInitialized() const { return (m_flags & Flags::Initialized) != 0; }

	bool getIsGPUReady() const { return (m_flags & Flags::GPUReady) != 0; }

	bool getIsSwapchain() const {return (m_flags & Flags::InitializedAsSwapchain) != 0; };

	u32 getMipMapCount() const;

	GfTextureFormat::Type getFormat() const;

	bool isDepthBuffer() const;

	bool isRT() const;

	u32 getWidth() const;

	u32 getHeight() const;

	u32 getDepth() const;

	u32 getSlices() const;

	TextureType getTextureType() const;

	TextureUsageFlags::Mask getTextureUsage() const;

	bool getIsMappable() const;

protected:

	u64 getDefaultViewID();

	u32 m_flags;
	TextureDesc m_desc;
	u64 m_defaultViewID;
};

////////////////////////////////////////////////////////////////////////////////

// Representation of a Texture 2D
class GfTexture2D : public GfTexture
{
public:

	using Id = s32;

	GfTexture2D();

	bool init(u32 width, u32 height, u32 mipCount, GfTextureFormat::Type format);

	////////////////////////////////////////////////////////////////////////////////
	// Commands

	void LoadTexture2DDataFromStaging(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiBufferOffset);

	////////////////////////////////////////////////////////////////////////////////
};

GF_FORCEINLINE u32 GfTexture::getMipMapCount() const
{
	return m_desc.m_mipCount;
}

GF_FORCEINLINE GfTextureFormat::Type GfTexture::getFormat() const
{
	return m_desc.m_format;
}

GF_FORCEINLINE bool GfTexture::isDepthBuffer() const
{
	return (m_desc.m_usage & TextureUsageFlags::DepthStencil) != 0;
}

GF_FORCEINLINE bool GfTexture::isRT() const
{
	return (m_desc.m_usage & TextureUsageFlags::ColorAttachment) != 0;
}

GF_FORCEINLINE u32 GfTexture::getWidth() const
{
	return m_desc.m_width;
}

GF_FORCEINLINE u32 GfTexture::getHeight() const
{
	return m_desc.m_height;
}

GF_FORCEINLINE u32 GfTexture::getDepth() const
{
	return m_desc.m_depth;
}

GF_FORCEINLINE u32 GfTexture::getSlices() const
{
	return m_desc.m_slices;
}

GF_FORCEINLINE TextureType GfTexture::getTextureType() const
{
	return m_desc.m_textureType;
}

GF_FORCEINLINE TextureUsageFlags::Mask GfTexture::getTextureUsage() const
{
	return m_desc.m_usage;
}

GF_FORCEINLINE bool GfTexture::getIsMappable() const
{
	return m_desc.m_mappable;
}

GF_FORCEINLINE void GfTexture2D::LoadTexture2DDataFromStaging(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiBufferOffset)
{
	m_kPlatform.loadTexture2DDataFromStagingBufferRHI(kCtx, kCmdBuffer, kFrom, uiBufferOffset);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTURE_H__