////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/17
//	File: 	GfConstantBuffer.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GraphicResources/GfConstantBuffer.h"

////////////////////////////////////////////////////////////////////////////////

GfConstantBuffer::GfConstantBuffer()
{
	m_eResourceType = EParamaterSlotType::ConstantBuffer;
}

////////////////////////////////////////////////////////////////////////////////

void* GfConstantBuffer::Map(const GfRenderContext& kCtx)
{
	if (m_kBufferRange.m_pBuffer && !m_kBufferRange.m_pBuffer->IsMapped())
	{
		return m_kBufferRange.m_pBuffer->Map(kCtx, m_kBufferRange.m_uiOffset, m_kBufferRange.m_uiSize);
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

void GfConstantBuffer::UnMap(const GfRenderContext& kCtx)
{
	if (m_kBufferRange.m_pBuffer && m_kBufferRange.m_pBuffer->IsMapped())
	{
		m_kBufferRange.m_pBuffer->UnMap(kCtx);
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF