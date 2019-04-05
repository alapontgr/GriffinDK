////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/18
//	File: 	GfTexture2D.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFTEXTURE2D_H__
#define __GFTEXTURE2D_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfRender/Common/GraphicResources/GfGraphicResourceBase.h"
#include GF_SOLVE_PLATFORM_HEADER_EXT(GfRender, GraphicResources/GfTexture2D)

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

////////////////////////////////////////////////////////////////////////////////

class GfTexture2D : public GfTexturedResource
{
	GF_DECLARE_PLATFORM_INTERFACE(GfTexture2D);
public:

	enum ETexture2DFlags : u16 
	{
		// User flags
		Mappable = 1<<0,
		Tilable	 = 1<<1,
	};

	static EParamaterSlotType::Type GetType() { return EParamaterSlotType::SampledTextured; }

	GfTexture2D();

	bool Init(u32 uiWidth, u32 uiHeight, u32 uiMips,
		ETextureFormat::Type eFormat,
		const ETextureUsageBits::GfMask& uiUsage,
		u16 uiFlags);

	void Create(const GfRenderContext& kCtx);

	void Destroy(const GfRenderContext& kCtx);

	bool IsMappable() const;

	bool IsDepthBuffer() const;

	bool IsStencilBuffer() const;

	bool IsTilable() const;

	u32 GetWidth() const;

	u32 GetHeight() const;

	u32 GetMipMapCount() const;

	////////////////////////////////////////////////////////////////////////////////
	// Commands

	void LoadTexture2DDataFromStaging(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiBufferOffset);

	////////////////////////////////////////////////////////////////////////////////

private:

	enum EPrivateFlags : u16
	{
		// Continue from public flags
		DepthBuffer		= (1<<2),
		StencilBuffer	= (1<<3),
	};

	GfBitMask<u16>				m_uiTextureFlags;
	ETextureUsageBits::GfMask	m_uiUsage;
	ETextureFormat::Type		m_eFormat;
	u32							m_uiMips;
	u32							m_uiWidth;
	u32							m_uiheight;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexture2D::IsMappable() const
{
	return (m_uiTextureFlags & ETexture2DFlags::Mappable) != 0;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexture2D::IsDepthBuffer() const
{
	return (m_uiTextureFlags & EPrivateFlags::DepthBuffer) != 0;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexture2D::IsStencilBuffer() const
{
	return (m_uiTextureFlags & EPrivateFlags::StencilBuffer) != 0;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexture2D::IsTilable() const
{
	return (m_uiTextureFlags & ETexture2DFlags::Tilable) != 0;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfTexture2D::GetWidth() const
{
	return m_uiWidth;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfTexture2D::GetHeight() const
{
	return m_uiheight;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfTexture2D::GetMipMapCount() const
{
	return m_uiMips;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfTexture2D::LoadTexture2DDataFromStaging(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiBufferOffset)
{
	m_kPlatform.LoadTexture2DDataFromStagingBufferRHI(kCtx, kCmdBuffer, kFrom, uiBufferOffset);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTURE2D_H__