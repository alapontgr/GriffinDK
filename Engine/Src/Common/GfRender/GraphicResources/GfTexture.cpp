////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/18
//	File: 	GfTexture.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GraphicResources/GfTexture.h"

////////////////////////////////////////////////////////////////////////////////

GfTexturedResource::GfTexturedResource()
	: m_kCommonPlatform(*this)
	, m_uiUsage(0)
	, m_uiMips(1)
	, m_eFormat(ETextureFormat::Undefined)
	, m_uiTextureFlags(0)
	, m_uiWidth(0)
	, m_uiHeight(0)
{
	m_uiGraphicResFlags.Enable(EGraphicResFlags::TexturedResource);
}

////////////////////////////////////////////////////////////////////////////////

void GfTexturedResource::ExternalInit(const GfExternTexInit& kInitParams)
{
	bool bInitialised(IsInitialised());
	if (!bInitialised || IsExternallyInitialized())
	{
		m_uiWidth = kInitParams.m_uiWidth;
		m_uiHeight = kInitParams.m_uiHeight;
		m_eFormat = kInitParams.m_eFormat;
		m_kCommonPlatform.ExternalInitPlat(kInitParams);
		MarkAsExternallyInitiailized();
		MarkAsInitialised();
		MarkAsGPUReady();
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfTexturedResource::SetUsage(const ETextureUsageBits::GfMask& uiUsage)
{
	m_uiUsage = uiUsage;
}

////////////////////////////////////////////////////////////////////////////////

void GfTexturedResource::SetFormat(ETextureFormat::Type eFormat)
{
	m_eFormat = eFormat;
	// Detect depth buffer
	if (eFormat >= ETextureFormat::D16_UNorm && eFormat <= ETextureFormat::D32_SFloat_S8_UInt)
	{
		if (!m_uiUsage.IsEnable(ETextureUsageBits::DepthStencilAttachment))
		{
			// Invalid configuration of texture
			return;
		}

		// Is depth buffer?
		if (eFormat != ETextureFormat::S8_UInt)
		{
			m_uiTextureFlags |= EPrivateFlags::DepthBuffer;
		}
		// Is stencil buffer?
		if (eFormat > ETextureFormat::D32_SFloat)
		{
			m_uiTextureFlags |= EPrivateFlags::StencilBuffer;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfTexturedResource::SetMips(u32 uiMipsCount)
{
	m_uiMips = uiMipsCount;
}

////////////////////////////////////////////////////////////////////////////////

void GfTexturedResource::SetTextureFlags(const GfFlagsMask& uiFlags)
{
	m_uiTextureFlags = uiFlags;
}

////////////////////////////////////////////////////////////////////////////////

void GfTexturedResource::SetDimensions(u32 uiWidth, u32 uiHeight)
{
	m_uiWidth = uiWidth;
	m_uiHeight = uiHeight;
}

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfTexture2D)
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
		SetDimensions(uiWidth, uiHeight);

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
	// Destroy only if needed. Do not worry if the texture was initialized with an external source
	if (IsGPUReady() && !IsExternallyInitialized())
	{
		m_kPlatform.DestroyRHI(kCtx);
		m_uiTextureFlags.Reset();
		MarkAsDestroyed();
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF