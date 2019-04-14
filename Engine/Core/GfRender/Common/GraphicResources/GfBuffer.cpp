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

void GfBuffer::Init(const GfBufferDesc& kDesc)
{
	if (!IsInitialised()) 
	{
		m_kDesc = kDesc;
		m_uiFlags.Enable(EFlag::Initialised);
	}
}

////////////////////////////////////////////////////////////////////////////////

bool GfBuffer::Create(const GfRenderContext& kCtxt)
{
	if (IsInitialised()) 
	{
		if (m_kPlatform.CreateRHI(kCtxt)) 
		{
			m_uiFlags.Enable(EFlag::GPUReady);
			return true;
		}	
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer::Destroy(const GfRenderContext& kCtxt)
{
	if (IsGPUReady()) 
	{
		m_kPlatform.DestroyRHI(kCtxt);
	}
	m_kDesc = GfBufferDesc();
	m_uiFlags = 0;
}

////////////////////////////////////////////////////////////////////////////////

void* GfBuffer::Map(const GfRenderContext& kCtxt, u32 uiOffset, u32 uiSize)
{
	GF_ASSERT(IsGPUReady(), "Buffer not created");
	GF_ASSERT((m_uiFlags & EFlag::Mapped) == 0, "Buffer is already mapped");
	if (IsMappable() && ((uiSize + uiOffset) <= m_kDesc.m_ulSize)) 
	{
		m_uiMappedOffset = uiOffset;
		m_uiMappedSize = uiSize;
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
		if (m_kDesc.m_uiMemoryProperties & EBufferMemProperties::CPU_GPU_Coherent) 
		{
			m_kPlatform.UnMapRHI(kCtxt);
		}
		else 
		{
			m_kPlatform.FlushAndUnMapRHI(kCtxt, m_uiMappedOffset, m_uiMappedSize);
			m_uiMappedSize = 0;
			m_uiMappedOffset = 0;
		}
		m_uiFlags.Disable(EFlag::Mapped);
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF