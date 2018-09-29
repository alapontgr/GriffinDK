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
#include GF_SOLVE_PLATFORM_HEADER(GfBuffer)
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

class GfBuffer : public GfBuffer_Platform
{
public:

	friend class GfBuffer_Platform;

	struct GfBufferDesc
	{
		size_t							m_ulSize			= 0;
		size_t							m_ulAlignment		= 1;
		GfBufferMemType					m_mMemoryProperties	= EBufferMemProperties::GPU_Local;
		GfBufferUsage					m_mBufferUsage		= EBufferUsage::InvalidUsage;
		EBufferUsage::Type				m_eBufferType		= EBufferUsage::InvalidUsage;
		GfRencerContextFamilies::Type	m_eTargetFamily		= GfRencerContextFamilies::Graphics;
	};

	////////////////////////////////////////////////////////////////////////////////

	struct GfRange
	{
		u32 m_uiOffset	= 0;
		u32 m_uiSize	= 0;
	};

	////////////////////////////////////////////////////////////////////////////////

	struct GfChunk
	{
		GfBuffer*	m_pBuffer		= nullptr;
		u32			m_uiDataOffset	= 0;
		u32			m_uiDataSize	= 0;
	};

	////////////////////////////////////////////////////////////////////////////////

	GfBuffer();

	bool Init(const GfRenderContext& kCtxt, const GfBufferDesc& kDesc);

	void Destroy(const GfRenderContext& kCtxt);

private:

	GfBufferDesc m_kDesc;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFBUFFER_H__