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

GF_DEFINE_BASE_CTOR(GfTexture2D)
	, m_uiWidth(0)
	, m_uiheight(0)
{
	// Set invalid type as this is not a valid resource to be bound
	m_eResourceType = EParamaterSlotType::Invalid;
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexture2D::Init(u32 uiWidth, u32 uiHeight, u32 uiMips, 
	ETextureFormat::Type eFormat, 
	const ETextureUsageBits::GfMask& uiUsage, 
	const GfTexturedResource::GfFlagsMask& uiFlags)
{
	if (!IsInitialised()) 
	{
		SetTextureFlags(uiFlags);
		SetMips(uiMips);
		SetUsage(uiUsage);
		SetFormat(eFormat);
		m_uiWidth = uiWidth;
		m_uiheight = uiHeight;
	
		if (eFormat >= ETextureFormat::D16_UNorm && eFormat <= ETextureFormat::D32_SFloat_S8_UInt)
		{
			if (!(uiUsage & ETextureUsageBits::DepthStencilAttachment))
			{
				// Invalid configuration of texture
				return false;
			}
		}

		// If it is a sampled image add the Transfer_Dst usage as its data will probably be loaded after the creation
		if ((uiUsage & ETextureUsageBits::Sampled) != 0) 
		{
			m_uiUsage.Enable(ETextureUsageBits::Transfer_Dst);
		}

		MarkAsInitialised();
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture2D::Create(const GfRenderContext& kCtx)
{
	if (IsInitialised()) 
	{
		if (!m_kPlatform.CreateRHI(kCtx))
		{
			m_kPlatform.DestroyRHI(kCtx);
		}
		else 
		{
			MarkAsGPUReady();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture2D::Destroy(const GfRenderContext& kCtx)
{
	m_kPlatform.DestroyRHI(kCtx);
	m_uiTextureFlags.Reset();
	MarkAsDestroyed();
}

////////////////////////////////////////////////////////////////////////////////
// EOF