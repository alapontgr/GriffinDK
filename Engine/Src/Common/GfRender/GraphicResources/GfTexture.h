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
#include "Common/GfRender/GraphicResources/GfGraphicResourceBase.h"
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

namespace TextureUsage 
{
	enum Type
	{
		Sample			= 1<<0,
		RenderTarget	= 1<<1,
		DepthStencil	= 1<<2,
		Storage			= 1<<3
	};
};
using TextureUsageMask = u32;

struct TextureDesc
{
	u32						m_width			= 0;
	u32						m_height		= 0;
	u32						m_depth			= 1;
	u32						m_slices		= 1;
	u32						m_mipCount		= 1;
	TextureNumSamples		m_numSamples	= TextureNumSamples::None;
	TextureType				m_textureType	= TextureType::Type_2D;
	TextureFormat::Type		m_format		= TextureFormat::R8G8B8A8_UNorm;
	TextureUsageMask		m_usage			= TextureUsage::Sample;
	bool					m_mappable		= false;
};

// Used to initialize swapchain iomages internally
struct SwapchainDesc : public GfExternTexInit_Platform 
{
	u32						m_width;
	u32						m_height;
	TextureFormat::Type		m_format = TextureFormat::R8G8B8A8_UNorm;
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
	void ExternalInit(const SwapchainDesc& kInitParams);

	bool getIsInitialized() const { return (m_flags & Flags::Initialized) != 0; }

	bool getIsGPUReady() const { return (m_flags & Flags::GPUReady) != 0; }

	bool getIsSwapchain() const {return (m_flags & Flags::InitializedAsSwapchain) != 0; };

	u32 getMipMapCount() const;

	TextureFormat::Type getFormat() const;

	bool isDepthBuffer() const;

	bool isRT() const;

	u32 getWidth() const;

	u32 getHeight() const;

	u32 getDepth() const;

	u32 getSlices() const;

	TextureType getTextureType() const;

	TextureUsageMask getTextureUsage() const;

	bool getIsMappable() const;

protected:

	u32 m_flags;
	TextureDesc m_desc;
};

////////////////////////////////////////////////////////////////////////////////

// Representation of a Texture 2D
class GfTexture2D : public GfTexture
{
public:

	using Id = s32;

	GfTexture2D();

	bool init(u32 width, u32 height, u32 mipCount, TextureFormat::Type format);

	////////////////////////////////////////////////////////////////////////////////
	// Commands

	void LoadTexture2DDataFromStaging(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiBufferOffset);

	////////////////////////////////////////////////////////////////////////////////
};

////////////////////////////////////////////////////////////////////////////////
// Inline Members

#include "Common/GfRender/GraphicResources/GfTexture.inl"

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTURE_H__