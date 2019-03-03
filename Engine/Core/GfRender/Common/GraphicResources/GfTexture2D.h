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

class GfTexture2D : public GfTexture2D_Platform, public GfGraphicsResourceBase
{
public:

	friend class GfTexture2D_Platform;

	enum EFlags : u16 
	{
		Mappable = (1<<0),
		Tilable	 = (1<<1),
	};

	static EParamaterSlotType::Type GetType() { return EParamaterSlotType::SampledImage; }

	GfTexture2D();

	bool Init(u32 uiWidth, u32 uiHeight, u32 uiMips,
		ETextureFormat::Type eFormat,
		const ETextureUsageBits::GfMask& uiUsage,
		u16 uiFlags);

	void Create(const GfRenderContext& kCtx);

	void Destroy(const GfRenderContext& kCtx);

	bool IsInitialised() const;

	bool IsMappable() const;

	bool IsDepthBuffer() const;

	bool IsStencilBuffer() const;

	bool IsTilable() const;

private:

	enum EPrivateFlags : u16 
	{
		// Continue from public flags
		DepthBuffer		= (1<<2),
		StencilBuffer	= (1<<3),
		Initialised		= (1<<4),
	};

	GfBitMask<u16>				m_uiFlags;
	ETextureUsageBits::GfMask	m_uiUsage;
	ETextureFormat::Type		m_eFormat;
	u32							m_uiMips;
	u32							m_uiWidth;
	u32							m_uiheight;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexture2D::IsInitialised() const
{
	return (m_uiFlags & EPrivateFlags::Initialised) != 0;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexture2D::IsMappable() const
{
	return (m_uiFlags & EFlags::Mappable) != 0;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexture2D::IsDepthBuffer() const
{
	return (m_uiFlags & EPrivateFlags::DepthBuffer) != 0;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexture2D::IsStencilBuffer() const
{
	return (m_uiFlags & EPrivateFlags::StencilBuffer) != 0;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexture2D::IsTilable() const
{
	return (m_uiFlags & EFlags::Tilable) != 0;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTURE2D_H__