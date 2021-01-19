////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/29
//	File: 	GfBuffer.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFBUFFER_H__
#define __GFBUFFER_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfCore/GfBitMask.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GraphicResources/GfBuffer_Platform.h)
#include "Common/GfRender/GfRenderConstants.h"

////////////////////////////////////////////////////////////////////////////////

namespace BufferType 
{
	enum Type : u32
	{
		Staging = 1,
		Vertex,
		Index,
		Uniform,
		Storage,
		// ...
	};
}

////////////////////////////////////////////////////////////////////////////////

class GfBuffer
{
	GF_DECLARE_PLATFORM_INTERFACE(GfBuffer);
public:

	friend class GfBuffer_Platform;

	using Id = s32;

	struct GfBufferDesc
	{
		BufferType::Type	m_bufferType;
		u32					m_size			= 0;
		u32					m_alignment		= 1;
		bool				m_mappable		= false;
	};

	struct GfRange 
	{
		GfBuffer*	m_pBuffer = nullptr;
		u32			m_uiOffset = 0;
		u32			m_uiSize = 0;
	};

	////////////////////////////////////////////////////////////////////////////////

	GfBuffer();

	void init(const GfBufferDesc& kDesc);

	bool create(const GfRenderContext& kCtxt);

	void destroy(const GfRenderContext& kCtxt);

	bool isMappable() const;

	bool isInitialised() const;

	bool isGPUReady() const;

	bool isMapped() const;

	void* map(const GfRenderContext& kCtxt, u32 uiOffset, u32 uiSize);
	
	void unMap(const GfRenderContext& kCtxt);

	////////////////////////////////////////////////////////////////////////////////
	// Commands

	void copyRangeFrom(const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiFromOffset, u32 uiToOffset, u32 uiSize);

	void updateRange(const GfCmdBuffer& kCmdBuffer, u32 uiOffset, u32 uiSize, void* pData);

	////////////////////////////////////////////////////////////////////////////////

private:

	enum EFlag : u32
	{
		Mapped = 1 << 0,
		Initialised = 1<<1,
		GPUReady = 1<<2,
		Mappable = 1<<3
	};

	GfBufferDesc	m_desc;
	u32				m_flags;
	u32				m_mappedOffset;
	u32				m_mappedSize;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfBuffer::copyRangeFrom(const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiFromOffset, u32 uiToOffset, u32 uiSize)
{
	m_kPlatform.copyRangeFromRHI(kCmdBuffer, kFrom, uiFromOffset, uiToOffset, uiSize);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfBuffer::updateRange(const GfCmdBuffer& kCmdBuffer, u32 uiOffset, u32 uiSize, void* pData)
{
	m_kPlatform.updateRangeRHI(kCmdBuffer, uiOffset, uiSize, pData);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfBuffer::isMappable() const
{
	return m_desc.m_mappable;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfBuffer::isInitialised() const
{
	return (m_flags & EFlag::Initialised);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfBuffer::isMapped() const
{
	return (m_flags & EFlag::Mapped);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfBuffer::isGPUReady() const
{
	return (m_flags & EFlag::GPUReady);
}



////////////////////////////////////////////////////////////////////////////////
#endif // __GFBUFFER_H__