#ifdef DEAD
////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/06
//	File: 	GfGraphicResources.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFGRAPHICRESOURCES_H__
#define __GFGRAPHICRESOURCES_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfRender/Common/GfMaterialShared.h"
#include "GfRender/Common/GfBuffer.h"
#include GF_SOLVE_PLATFORM_HEADER(GfGraphicResources)

////////////////////////////////////////////////////////////////////////////////

class GfGraphicsResource 
{
public:

	GfGraphicsResource();

	GfGraphicsResource(EParamaterSlotType::Type eType);

	EParamaterSlotType::Type GetResourceType() const { return m_eType; }

protected:

	enum class EFlags : u32 
	{
		BufferBound = 1<<0,
		GPUDirty	= 1<<1,
		GPUPending	= 1<<2,
	};

	GfBitMask<u16>				m_uiFlags;
	EParamaterSlotType::Type	m_eType;
};

////////////////////////////////////////////////////////////////////////////////

class GfConstantBuffer : public GfGraphicsResource
{
public:

	GfConstantBuffer();

	void BindBuffer(const GfBuffer::GfRange& kRange);

	void Init(const GfRenderContext& kCtxt);

	u32 GetSize() const { return m_kBufferRange.m_uiSize; }

private:

	GfBuffer::GfRange m_kBufferRange;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFGRAPHICRESOURCES_H__
#endif // DEAD