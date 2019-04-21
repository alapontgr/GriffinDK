////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/06
//	File: 	GfGraphicResourceBase.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GraphicResources/GfGraphicResourceBase.h"

////////////////////////////////////////////////////////////////////////////////

GfGraphicsResourceBase::GfGraphicsResourceBase()
	: m_eResourceType(EParamaterSlotType::Invalid)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfGraphicsResourceBase::MarkAsDestroyed()
{
	m_uiGraphicResFlags.Disable(EGraphicResFlags::Initialised | EGraphicResFlags::GPUReady);
}

////////////////////////////////////////////////////////////////////////////////

void GfGraphicsResourceBase::MarkAsGPUReady()
{
	m_uiGraphicResFlags.Enable(EGraphicResFlags::GPUReady);
}

////////////////////////////////////////////////////////////////////////////////

void GfGraphicsResourceBase::MarkAsInitialised()
{
	m_uiGraphicResFlags.Enable(EGraphicResFlags::Initialised);
}

////////////////////////////////////////////////////////////////////////////////

GfBufferedResource::GfBufferedResource()
{
	m_uiGraphicResFlags.Enable(EGraphicResFlags::BufferedResource);
}

////////////////////////////////////////////////////////////////////////////////

void GfBufferedResource::BindBuffer(const GfBuffer::GfRange& kRange)
{
	if (!m_uiGraphicResFlags.IsEnable(EBUfferedResFlags::BufferBound))
	{
		m_kBufferRange = kRange;
		m_uiGraphicResFlags |= (EBUfferedResFlags::BufferBound);
	}
}

////////////////////////////////////////////////////////////////////////////////

GfTexturedResource::GfTexturedResource()
	: m_kCommonPlatform(*this)
	, m_uiUsage(0)
	, m_uiMips(1)
	, m_eFormat(ETextureFormat::Undefined)
	, m_uiTextureFlags(0)
{
	m_uiGraphicResFlags.Enable(EGraphicResFlags::TexturedResource);
}

////////////////////////////////////////////////////////////////////////////////

void GfTexturedResource::SetUsage(const ETextureUsageBits::GfMask& uiUsage)
{
	m_uiUsage = uiUsage;
}

////////////////////////////////////////////////////////////////////////////////

void GfTexturedResource::SetFormat(ETextureFormat::Type eFormat)
{
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
// EOF
