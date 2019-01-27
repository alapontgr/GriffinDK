////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/07
//	File: 	GfMaterialShared.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMATERIALSHARED_H__
#define __GFMATERIALSHARED_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfCore/Common/GfBitMask.h"

////////////////////////////////////////////////////////////////////////////////

namespace EParamaterSlotType
{
	enum Type : u32
	{
		Sampler = 0,
		CombinedImageSampler,
		SampledImage,
		StorageImage,
		UniformTexelBuffer,
		StorageTexelBuffer,
		UniformBuffer,
		StorageBuffer,
		UniformBufferDynamic,
		StorageBufferDynamic,

		////////////////////////////////////////////////////////////////////////////////
		Count,
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EShaderStageFlags
{
	enum Type : u32
	{
		Vertex = 1 << 0,
		TesellationControl = 1 << 1,
		TesellationEval = 1 << 2,
		Geometry = 1 << 3,
		Fragment = 1 << 4,
		Compute = 1 << 5,
		// All the stages within the graphics pipeline
		AllGraphics = Vertex | TesellationControl | TesellationEval | Geometry | Fragment,
		////////////////////////////////////////////////////////////////////////////////
		COUNT = 6 // IMPORTANT: Update if the table changes
	};
}
using GfShaderAccessMask = GfBitMask<u32>;

////////////////////////////////////////////////////////////////////////////////

namespace EMaterialParamRate 
{
	enum Type : u32 
	{
		PerFrame = 0,			// Common to all the objects of a frame
		PerRenderPass,			// Resources local to a RenderPass
		PerMaterial,			// Shared within all the instances of the same material
		PerInstance,			// Local configuration of each instance
		////////////////////////////////////////////////////////////////////////////////
		Count,
		////////////////////////////////////////////////////////////////////////////////
		MaxBoundSets = 8,		// Maximum number of currently bound descriptor sets in a material instance

		Invalid = 0xffffffff
	};
}
static_assert(EMaterialParamRate::Count <= EMaterialParamRate::MaxBoundSets, "Invalid count of rates");

////////////////////////////////////////////////////////////////////////////////

namespace EPrimitiveTopology 
{
	enum Type : u32 
	{
		PointList = 0,
		LineList,
		LineStrip,
		TriList,
		TriStrip,
		TriFan,
		LineListWithAdj,
		LineStripWithAdj,
		TriListWithAdj,
		TriStripWithAdj,
		PatchList,
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EPolygonMode 
{
	enum Type : u8
	{
		Fill = 0,
		Line,
		Point
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace ECullMode 
{
	enum Type : u8
	{
		None,
		Front,
		Back, 
		FrontBack,
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EFrontFace 
{
	enum Type : u8 
	{
		CounterClockwise = 0,
		Clockwise,
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EMultiSampleCount 
{
	enum Type : u8 
	{
		Samples_1 = 0,
		Samples_2,
		Samples_4,
		Samples_8,
		Samples_16,
		Samples_32,
		Samples_64,
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EBlendFactor 
{
	enum Type : u8 
	{
		Zero = 0,
		One,
		Src_Color,
		One_Minus_Src_Color,
		Dst_Color,
		One_Minus_Dst_Color,
		Src_Alpha,
		One_Minus_Src_Alpha,
		Dst_Alpha,
		One_Minus_Dst_Alpha,
		Const_Color,
		One_Minus_Const_Color,
		Const_Alpha,
		One_Minus_Const_Alpha,
		Src_Alpha_Saturate,
		Src1_Color,
		One_Minus_Src1_Color,
		Src1_Alpha,
		One_Minus_Src1_Alpha,
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EBlendOp 
{
	enum Type : u8 
	{
		Add = 0,
		Subtract,
		Reverse_Subtract,
		Min,
		Max,
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EVertexAttribute 
{
	enum Type : u16
	{
		Float = 0,	// R32
		Float2,		// R32G32
		Float3,		// R32G32B32
		Float4,		// R32G32B32A32

		SInt,		// R32
		SInt2,		// R32G32
		SInt3,		// R32G32B32
		SInt4,		// R32G32B32A32

		UInt,		// R32
		UInt2,		// R32G32
		UInt3,		// R32G32B32
		UInt4		// R32G32B32A32
	};
}

////////////////////////////////////////////////////////////////////////////////

struct GfRasterState 
{
	f32                 m_fDepthBiasConstFactor			= 0.0f;
	f32                 m_fDepthBiasClamp				= 0.0f;
	f32                 m_fDepthBiasSlopeFactor			= 0.0f;
	f32                 m_fLineWidth					= 1.0f;
	EPolygonMode::Type	m_ePolygonMode					= EPolygonMode::Fill;
	ECullMode::Type		m_eCullMode						= ECullMode::Back;
	EFrontFace::Type	m_eFrontFace					= EFrontFace::Clockwise;
	bool				m_bDepthClampEnabled			= false;
	bool				m_bRasterizerDiscardEnabled		= false;
	bool				m_bDepthBiasEnabled				= false;
};

////////////////////////////////////////////////////////////////////////////////

// TODO: MultiSampling config

struct GfMultiSamplingState 
{
	EMultiSampleCount::Type m_uiSampleCount = EMultiSampleCount::Samples_1;
	bool					m_bEnabled		= false;
};

////////////////////////////////////////////////////////////////////////////////

struct GfBlendState 
{
	bool				m_bEnabled				= false;
	EBlendFactor::Type	m_eSrcColorBlendFactor;
	EBlendFactor::Type	m_eDstColorBlendFactor;
	EBlendOp::Type		m_eColorBlendOp;
	EBlendFactor::Type	m_eSrcAlphaBlendFactor;
	EBlendFactor::Type	m_eDstAlphaBlendFactor;
	EBlendOp::Type		m_eAlphaBlendOp;
};

////////////////////////////////////////////////////////////////////////////////

class GfVertexDeclaration 
{
public:

	struct GfVertexAttrib 
	{
		EVertexAttribute::Type	m_eType;	// Type of the attribute
		u16						m_uiOffset;	// Offset within the struct where this attrib is
	};

	GfVertexDeclaration();

	void Init(GfVertexAttrib* pAttributes, u16 uiAttribCount, u16 uiStride);

	static constexpr u32 ms_uiVertexMaxAttribCount = 6;

private:

	GfVertexAttrib m_pAttribs[ms_uiVertexMaxAttribCount];
	u16 m_uiAttribCount;
	u16 m_uiVertexStride;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIALSHARED_H__