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

	};

	static EParamaterSlotType::Type GetType() { return EParamaterSlotType::SampledImage; }

	void Init(u32 uiWidth, u32 uiHeight, u32 uiMips,
		ETextureFormat::Type eFormat,
		const ETextureUsageBits::GfMask& uiUsage,
		bool bTiling = true);

	void Create(const GfRenderContext& kCtx);

	GfTexture2D();

private:

	GfBitMask<u16>				m_uiFlags;
	ETextureUsageBits::GfMask	m_uiUsage;
	ETextureFormat::Type		m_eFormat;
	u32							m_uiMips;
	u32							m_uiWidth;
	u32							m_uiheight;
	bool						m_bUseTiling;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTURE2D_H__