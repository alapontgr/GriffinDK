////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/18
//	File: 	GfTexture2D.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GraphicResources/GfTexture2D.h"

////////////////////////////////////////////////////////////////////////////////

GfTexture2D::GfTexture2D()
	: GfTexture2D_Platform(*this)
	, GfGraphicsResourceBase(GetType())
	, m_uiFlags(0)
	, m_uiUsage(0)
	, m_eFormat(ETextureFormat::Undefined)
	, m_uiMips(0)
	, m_uiWidth(0)
	, m_uiheight(0)
{
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexture2D::Init(u32 uiWidth, u32 uiHeight, u32 uiMips, ETextureFormat::Type eFormat, const ETextureUsageBits::GfMask& uiUsage, u16 uiFlags)
{
	if (!IsInitialised()) 
	{
		m_uiFlags = uiFlags;
		m_uiUsage = uiUsage;
		m_eFormat = eFormat;
		m_uiMips = uiMips;
		m_uiWidth = uiWidth;
		m_uiheight = uiHeight;

		// Detect depth buffer
		if (eFormat >= ETextureFormat::D16_UNorm && eFormat <= ETextureFormat::D32_SFloat_S8_UInt)
		{
			if (!(uiUsage & ETextureUsageBits::DepthStencilAttachment))
			{
				// Invalid configuration of texture
				return false;
			}

			// Is depth buffer?
			if (eFormat != ETextureFormat::S8_UInt)
			{
				m_uiFlags |= EPrivateFlags::DepthBuffer;
			}
			// Is stencil buffer?
			if (eFormat > ETextureFormat::D32_SFloat) 
			{
				m_uiFlags |= EPrivateFlags::StencilBuffer;
			}
		}
	
		// If it is a sampled image add the Transfer_Dst usage as its data will probably be loaded after the creation
		if ((uiUsage & ETextureUsageBits::Sampled) != 0) 
		{
			m_uiUsage |= ETextureUsageBits::Transfer_Dst;
		}

		m_uiFlags |= EPrivateFlags::Initialised;
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture2D::Create(const GfRenderContext& kCtx)
{
	if (IsInitialised()) 
	{
		if (!CreateRHI(kCtx)) 
		{
			DestroyRHI(kCtx);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture2D::Destroy(const GfRenderContext& kCtx)
{
	DestroyRHI(kCtx);
	m_uiFlags = 0;
}

////////////////////////////////////////////////////////////////////////////////
// EOF