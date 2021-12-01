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

bool GfBuffer::create(const GfRenderContext& kCtxt, const GfBufferDesc& desc)
{
	if (!isInitialised()) 
	{
		m_desc = desc;
		m_flags |= EFlag::Initialised;
		if (m_kPlatform.createRHI(kCtxt)) 
		{
			m_flags |= EFlag::GPUReady;
			return true;
		}	
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer::destroy(const GfRenderContext& ctx)
{
	if (isGPUReady()) 
	{
		m_kPlatform.destroyRHI(ctx);
	}
	m_desc = GfBufferDesc();
	m_flags = 0;
}

////////////////////////////////////////////////////////////////////////////////

void* GfBuffer::map(const GfRenderContext& kCtxt, u32 uiOffset, u32 uiSize)
{
	GF_ASSERT(isGPUReady(), "Buffer not created");
	GF_ASSERT(!isMapped(), "Buffer is already mapped");
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
	if (isMapped()) 
	{
		m_kPlatform.unMapRHI(kCtxt);
		m_mappedSize = 0;
		m_mappedOffset = 0;
		m_flags &= ~EFlag::Mapped;
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF