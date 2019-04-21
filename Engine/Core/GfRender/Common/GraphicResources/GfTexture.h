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

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfRender/Common/GraphicResources/GfGraphicResourceBase.h"
#include GF_SOLVE_PLATFORM_HEADER_EXT(GfRender, GraphicResources/GfTexture)

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

////////////////////////////////////////////////////////////////////////////////

// Base class for texture types
class GfTexturedResource : public GfGraphicsResourceBase
{
public:

	enum ETexture2DFlags : u16
	{
		// User flags
		Mappable = 1 << 0,
		Tilable = 1 << 1,
	};
	using GfFlagsMask = GfBitMask<u16>;

	GfTexturedResource();

	GfTexturedResource_Platform& GetSharedPlatform();
	const GfTexturedResource_Platform& GetSharedPlatformC() const;

	u32 GetMipMapCount() const;

	ETextureFormat::Type GetFormat() const;

	bool IsUsageValid(ETextureUsageBits::Type eUsage) const;

	bool IsMappable() const;

	bool IsDepthBuffer() const;

	bool IsStencilBuffer() const;

	bool IsTilable() const;

	u32 GetWidth() const;

	u32 GetHeight() const;

protected:

	enum EPrivateFlags : u16
	{
		// Continue from public flags
		DepthBuffer = (1 << 2),
		StencilBuffer = (1 << 3),
		ExternallyInitialised = (1 << 4),
	};

	void SetUsage(const ETextureUsageBits::GfMask& uiUsage);
	void SetFormat(ETextureFormat::Type eFormat);
	void SetMips(u32 uiMipsCount);
	void SetTextureFlags(const GfFlagsMask& uiFlags);
	void SetDimensions(u32 uiWidth, u32 uiHeight);

	GfTexturedResource_Platform m_kCommonPlatform;
	ETextureUsageBits::GfMask	m_uiUsage;
	u32							m_uiMips;
	ETextureFormat::Type		m_eFormat;
	GfFlagsMask					m_uiTextureFlags;
	// Resolution
	u32							m_uiWidth;
	u32							m_uiHeight;
};

////////////////////////////////////////////////////////////////////////////////

// Representation of a Texture 2D
class GfTexture2D : public GfTexturedResource
{
	GF_DECLARE_PLATFORM_INTERFACE(GfTexture2D);
public:

	GfTexture2D();

	bool Init(u32 uiWidth, u32 uiHeight, u32 uiMips,
		ETextureFormat::Type eFormat,
		const ETextureUsageBits::GfMask& uiUsage,
		const GfTexturedResource::GfFlagsMask& uiFlags);

	void Create(const GfRenderContext& kCtx);

	void Destroy(const GfRenderContext& kCtx);

	////////////////////////////////////////////////////////////////////////////////
	// Commands

	void LoadTexture2DDataFromStaging(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiBufferOffset);

	////////////////////////////////////////////////////////////////////////////////
};

////////////////////////////////////////////////////////////////////////////////
// Inline Members

#include "GfRender/Common/GraphicResources/GfTexture.inl"

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTURE_H__