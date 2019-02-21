////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/17
//	File: 	GfConstantBuffer.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFCONSTANTBUFFER_H__
#define __GFCONSTANTBUFFER_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfRender/Common/GraphicResources/GfGraphicResourceBase.h"
#include "GfRender/Common/GraphicResources/GfBuffer.h"

////////////////////////////////////////////////////////////////////////////////

class GfConstantBuffer : public GfGraphicsResourceBase
{
public:

	static EParamaterSlotType::Type GetType() { return EParamaterSlotType::UniformBuffer; }

	GfConstantBuffer();

	void BindBuffer(const GfBuffer::GfRange& kRange);

	void Init(const GfRenderContext& kCtxt);

	u32 GetSize() const { return m_kBufferRange.m_uiSize; }

	const GfBuffer::GfRange GetBufferRange() const { return m_kBufferRange; }

private:

	GfBuffer::GfRange m_kBufferRange;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCONSTANTBUFFER_H__