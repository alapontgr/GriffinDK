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

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfCore/Common/GfBitMask.h"
#include GF_SOLVE_PLATFORM_HEADER_EXT(GfRender, GraphicResources/GfBuffer)
#include "GfRender/Common/GfRenderConstants.h"

////////////////////////////////////////////////////////////////////////////////

namespace EBufferUsage 
{
	enum Type : u32
	{
		InvalidUsage		= 0,
		Transfer_Src		= 1 << 0,
		Transfer_Dst		= 1 << 1,
		Uniform_Texel		= 1 << 2,
		Storage_Texel		= 1 << 3,
		Uniform_Buffer		= 1 << 4,
		Storage_Buffer		= 1 << 5,
		Index_Buffer		= 1 << 6,
		Vertex_Buffer		= 1 << 7,
		Indirect_Buffer		= 1 << 8,
	};
}
// Combination of flags defined in EBufferUsage
using GfBufferUsage = GfBitMask<u32>;

////////////////////////////////////////////////////////////////////////////////

namespace EBufferMemProperties 
{
	enum Type : u32
	{
		GPU_Local			= 1 << 0,
		CPU_Visible			= 1 << 1,
		CPU_GPU_Coherent	= 1 << 2,
		CPU_Cached			= 1 << 3,
		Lazy_Allocated		= 1 << 4
	};
}
// Combination of flags defined in EBufferMemProperties
using GfBufferMemType = GfBitMask<u32>;

////////////////////////////////////////////////////////////////////////////////

class GfBuffer
{
	GF_DECLARE_PLATFORM_INTERFACE(GfBuffer);
public:

	friend class GfBuffer_Platform;

	struct GfBufferDesc
	{
		size_t							m_ulSize			= 0;
		size_t							m_ulAlignment		= 1;
		GfBufferMemType					m_uiMemoryProperties	= EBufferMemProperties::GPU_Local;
		GfBufferUsage					m_uiBufferUsage		= EBufferUsage::InvalidUsage;
		EBufferUsage::Type				m_eBufferType		= EBufferUsage::InvalidUsage;
		// GfRencerContextFamilies::Type	m_eTargetFamily		= GfRencerContextFamilies::Graphics;
	};

	////////////////////////////////////////////////////////////////////////////////

	struct GfRange 
	{
		GfBuffer*	m_pBuffer = nullptr;
		u32			m_uiOffset = 0;
		u32			m_uiSize = 0;
	};

	////////////////////////////////////////////////////////////////////////////////

	GfBuffer();

	void Init(const GfBufferDesc& kDesc);

	bool Create(const GfRenderContext& kCtxt);

	void Destroy(const GfRenderContext& kCtxt);

	bool IsMappable() const;

	bool IsInitialised() const;

	bool IsGPUReady() const;

	void* Map(const GfRenderContext& kCtxt, u32 uiOffset, u32 uiSize);
	
	void UnMap(const GfRenderContext& kCtxt);

	////////////////////////////////////////////////////////////////////////////////
	// Commands

	void CopyRange(const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, const GfBuffer& kTo, u32 uiFromOffset, u32 uiToOffset, u32 uiSize);

	void UpdateRange(const GfCmdBuffer& kCmdBuffer, const GfBuffer& kBuffer, u32 uiOffset, u32 uiSize, void* pData);

	////////////////////////////////////////////////////////////////////////////////

private:

	enum EFlag : u32
	{
		Mapped = 1 << 0,
		Initialised = 1<<1,
		GPUReady = 1<<2
	};

	GfBufferDesc	m_kDesc;
	GfBitMask<u32>	m_uiFlags;
	u32				m_uiMappedOffset;
	u32				m_uiMappedSize;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfBuffer::CopyRange(const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, const GfBuffer& kTo, u32 uiFromOffset, u32 uiToOffset, u32 uiSize)
{
	m_kPlatform.CopyRangeRHI(kCmdBuffer, kFrom, kTo, uiFromOffset, uiToOffset, uiSize);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfBuffer::UpdateRange(const GfCmdBuffer& kCmdBuffer, const GfBuffer& kBuffer, u32 uiOffset, u32 uiSize, void* pData)
{
	m_kPlatform.UpdateRangeRHI(kCmdBuffer, kBuffer, uiOffset, uiSize, pData);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfBuffer::IsMappable() const
{
	return (m_kDesc.m_uiMemoryProperties & EBufferMemProperties::CPU_Visible) != 0;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfBuffer::IsInitialised() const
{
	return m_uiFlags.IsEnable(EFlag::Initialised);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfBuffer::IsGPUReady() const
{
	return m_uiFlags.IsEnable(EFlag::GPUReady);
}



////////////////////////////////////////////////////////////////////////////////
#endif // __GFBUFFER_H__