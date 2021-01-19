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

#include "Common/GfRender/GraphicResources/GfBuffer.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfBuffer)
	, m_flags(0)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer::init(const GfBufferDesc& kDesc)
{
	if (!isInitialised()) 
	{
		m_desc = kDesc;
		m_flags |= EFlag::Initialised;
		if (m_desc.m_mappable) 
		{
			m_flags |= EFlag::Mappable;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

bool GfBuffer::create(const GfRenderContext& kCtxt)
{
	if (isInitialised()) 
	{
		if (m_kPlatform.CreateRHI(kCtxt)) 
		{
			m_flags |= EFlag::GPUReady;
			return true;
		}	
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer::destroy(const GfRenderContext& kCtxt)
{
	if (isGPUReady()) 
	{
		m_kPlatform.DestroyRHI(kCtxt);
	}
	m_desc = GfBufferDesc();
	m_flags = 0;
}

////////////////////////////////////////////////////////////////////////////////

void* GfBuffer::map(const GfRenderContext& kCtxt, u32 uiOffset, u32 uiSize)
{
	GF_ASSERT(isGPUReady(), "Buffer not created");
	GF_ASSERT((m_flags & EFlag::Mapped) == 0, "Buffer is already mapped");
	if (isMappable() && ((uiSize + uiOffset) <= m_desc.m_size)) 
	{
		m_mappedOffset = uiOffset;
		m_mappedSize = uiSize;
		m_flags |= EFlag::Mapped;
		return m_kPlatform.MapRHI(kCtxt, uiOffset, uiSize);
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer::unMap(const GfRenderContext& kCtxt)
{
	if (m_flags & EFlag::Mapped) 
	{
		m_kPlatform.unMapRHI(kCtxt);
		m_mappedSize = 0;
		m_mappedOffset = 0;
		m_flags &= ~EFlag::Mapped;
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF