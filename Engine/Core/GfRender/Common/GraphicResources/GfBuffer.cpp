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

GF_DEFINE_BASE_CTOR(GfBuffer)
	, m_uiFlags(0)
{
}

////////////////////////////////////////////////////////////////////////////////

bool GfBuffer::Init(const GfRenderContext& kCtxt, const GfBufferDesc& kDesc)
{
	Destroy(kCtxt);
	m_kDesc = kDesc;
	return m_kPlatform.InitRHI(kCtxt);
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer::Destroy(const GfRenderContext& kCtxt)
{
	if (m_kDesc.m_ulSize > 0) 
	{
		m_kPlatform.DestroyRHI(kCtxt);
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
		return m_kPlatform.MapRHI(kCtxt, uiOffset, uiSize);
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer::UnMap(const GfRenderContext& kCtxt)
{
	if (m_uiFlags & EFlag::Mapped) 
	{
		m_kPlatform.UnMapRHI(kCtxt);
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF