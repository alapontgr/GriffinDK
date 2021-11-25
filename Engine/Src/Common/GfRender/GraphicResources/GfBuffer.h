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
#include "Common/GfRender/GfRenderCommon.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GraphicResources/GfBuffer_Platform.h)

////////////////////////////////////////////////////////////////////////////////

class GfBuffer
{
	GF_DECLARE_PLATFORM_INTERFACE(GfBuffer);
public:

	friend class GfBuffer_Platform;

	using Id = s32;

	struct GfBufferDesc
	{
		BufferUsageFlags::Mask	m_bufferType;
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

	void init(const GfBufferDesc& desc);

	bool create(const GfRenderContext& ctx);

	bool create(const GfRenderContext& ctx, const GfBufferDesc& desc);

	void destroy(const GfRenderContext& ctx);

	bool isMappable() const;

	bool isInitialised() const;

	bool isGPUReady() const;

	bool isMapped() const;

	void* map(const GfRenderContext& ctx, u32 offset, u32 size);
	
	void unMap(const GfRenderContext& ctx);

	////////////////////////////////////////////////////////////////////////////////
	// Commands

	void copyRangeFrom(const GfCmdBuffer& cmdBuffer, const GfBuffer& from, u32 fromOffset, u32 toOffset, u32 size);

	void updateRange(const GfCmdBuffer& cmdBuffer, u32 offset, u32 size, void* data);

	////////////////////////////////////////////////////////////////////////////////

private:

	enum EFlag : u32
	{
		Mapped = 1 << 0,
		Initialised = 1<<1,
		GPUReady = 1<<2
	};

	GfBufferDesc	m_desc;
	u32				m_flags;
	u32				m_mappedOffset;
	u32				m_mappedSize;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfBuffer::copyRangeFrom(const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 srcOffset, u32 dstOffset, u32 uiSize)
{
	m_kPlatform.copyRangeFromRHI(kCmdBuffer, kFrom, srcOffset, dstOffset, uiSize);
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
	return (m_flags & EFlag::Initialised) == EFlag::Initialised;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfBuffer::isMapped() const
{
	return (m_flags & EFlag::Mapped) == EFlag::Mapped;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfBuffer::isGPUReady() const
{
	return (m_flags & EFlag::GPUReady) == EFlag::GPUReady;
}



////////////////////////////////////////////////////////////////////////////////
#endif // __GFBUFFER_H__