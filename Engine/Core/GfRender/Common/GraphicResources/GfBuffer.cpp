////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/29
//	File: 	GfBuffer.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GraphicResources/GfBuffer.h"

////////////////////////////////////////////////////////////////////////////////

GfBuffer::GfBuffer()
	: GfBuffer_Platform(*this)
	, m_uiFlags(0)
{
}

////////////////////////////////////////////////////////////////////////////////

bool GfBuffer::Init(const GfRenderContext& kCtxt, const GfBufferDesc& kDesc)
{
	Destroy(kCtxt);
	m_kDesc = kDesc;
	return InitPlatform(kCtxt);
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer::Destroy(const GfRenderContext& kCtxt)
{
	if (m_kDesc.m_ulSize > 0) 
	{
		DestroyPlatform(kCtxt);
		m_kDesc = GfBufferDesc();
	}
}

////////////////////////////////////////////////////////////////////////////////

void* GfBuffer::Map(const GfRenderContext& kCtxt, u32 uiOffset, u32 uiSize)
{
	GF_ASSERT((m_uiFlags & EFlag::Mapped) != 0, "Buffer is already mapped");
	if (IsMappable() && ((uiSize + uiOffset) < m_kDesc.m_ulSize)) 
	{
		m_uiFlags |= EFlag::Mapped;
		return MapRHI(kCtxt, uiOffset, uiSize);
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer::UnMap(const GfRenderContext& kCtxt)
{
	if (m_uiFlags & EFlag::Mapped) 
	{
		UnMapRHI(kCtxt);
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF