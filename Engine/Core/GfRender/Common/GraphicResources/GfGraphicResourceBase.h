////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/06
//	File: 	GfGraphicResourceBase.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFGRAPHICRESOURCEBASE_H__
#define __GFGRAPHICRESOURCEBASE_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfRender/Common/GfMaterialShared.h"
#include "GfRender/Common/GraphicResources/GfGraphicResourcesShared.h"
#include "GfRender/Common/GraphicResources/GfBuffer.h"

////////////////////////////////////////////////////////////////////////////////

class GfGraphicsResourceBase 
{
public:

	GfGraphicsResourceBase();

	bool IsBufferedResource() const;

	bool IsTexturedResource() const;

	EParamaterSlotType::Type GetResourceType() const;

	bool IsGPUReady() const;

	bool IsInitialised() const;

protected:

	enum EGraphicResFlags : u32 
	{
		GPUReady			= 1<<0,
		Initialised			= 1<<1,
		BufferedResource	= 1<<2,
		TexturedResource	= 1<<3,
		////////////////////////////////////////////////////////////////////////////////
		End					= 4,
	};

	void MarkAsDestroyed();

	void MarkAsGPUReady();

	void MarkAsInitialised();

	EParamaterSlotType::Type m_eResourceType;
	GfBitMask<u32>	m_uiGraphicResFlags;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE EParamaterSlotType::Type GfGraphicsResourceBase::GetResourceType() const
{
	return m_eResourceType;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfGraphicsResourceBase::IsBufferedResource() const
{
	return m_uiGraphicResFlags.IsEnable(EGraphicResFlags::BufferedResource);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfGraphicsResourceBase::IsTexturedResource() const
{
	return m_uiGraphicResFlags.IsEnable(EGraphicResFlags::TexturedResource);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfGraphicsResourceBase::IsGPUReady() const 
{
	return m_uiGraphicResFlags.IsEnable(EGraphicResFlags::GPUReady);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfGraphicsResourceBase::IsInitialised() const
{
	return m_uiGraphicResFlags.IsEnable(EGraphicResFlags::Initialised);
}

////////////////////////////////////////////////////////////////////////////////

class GfBufferedResource : public GfGraphicsResourceBase
{
public:

	GfBufferedResource();

	void BindBuffer(const GfBuffer::GfRange& kRange);

	u32 GetSize() const { return m_kBufferRange.m_uiSize; }

	const GfBuffer::GfRange GetBufferRange() const { return m_kBufferRange; }

protected:

	enum EBUfferedResFlags : u32 
	{
		BufferBound = 1<<GfGraphicsResourceBase::EGraphicResFlags::End,
	};

	GfBuffer::GfRange m_kBufferRange;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFGRAPHICRESOURCEBASE_H__