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
	enum Type : u8
	{
		Sampler = 0,
		CombinedImageSampler,	// Texture 2D
		SampledImage,			// Texture 2D
		StorageImage,
		UniformTexelBuffer,
		StorageTexelBuffer,
		UniformBuffer,			// Constant Buffer
		StorageBuffer,
		UniformBufferDynamic,
		StorageBufferDynamic,

		////////////////////////////////////////////////////////////////////////////////
		Count,
		Invalid = 0xff,
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EShaderStage 
{
	enum Type : u32 
	{
		Vertex = 0,
		TesellationControl,
		TesellationEval,
		Geometry,
		Fragment,
		Compute,
		////////////////////////
		COUNT
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EShaderStageFlags
{
	enum FlagBit : u32
	{
		Vertex				= 1 << EShaderStage::Vertex,
		TesellationControl	= 1 << EShaderStage::TesellationControl,
		TesellationEval		= 1 << EShaderStage::TesellationEval,
		Geometry			= 1 << EShaderStage::Geometry,
		Fragment			= 1 << EShaderStage::Fragment,
		Compute				= 1 << EShaderStage::Compute,
		// All the stages within the graphics pipeline
		AllGraphics = Vertex | TesellationControl | TesellationEval | Geometry | Fragment,
	};
}
using GfShaderAccessMask = GfBitMask<u32>;

////////////////////////////////////////////////////////////////////////////////

namespace EMaterialParamRate 
{
	enum Type : u8 
	{
		PerFrame = 0,			// Common to all the objects of a frame
		PerRenderPass,			// Resources local to a RenderPass
		PerMaterial,			// Shared within all the instances of the same material
		PerInstance,			// Local configuration of each instance
		////////////////////////////////////////////////////////////////////////////////
		Count,
		////////////////////////////////////////////////////////////////////////////////
		MaxBoundSets = 8,		// Maximum number of currently bound descriptor sets in a material instance

		Invalid = 0xff
	};
}
static_assert(EMaterialParamRate::Count <= EMaterialParamRate::MaxBoundSets, "Invalid count of rates");

////////////////////////////////////////////////////////////////////////////////

namespace EPrimitiveTopology 
{
	enum Type : u8 
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

namespace EAttributeFormat 
{
	enum Type : u8
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

namespace EVertexInputRate 
{
	enum Type : u8 
	{
		PerVertex = 0,
		PerInstance
	};
}

////////////////////////////////////////////////////////////////////////////////

/*
	See https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkLogicOp.html
*/
namespace EBlendLogicOp 
{
	enum Type : u8 
	{
		Clear,			
		And,			
		AndReversed,	
		Copy,			
		AndInverted,	
		NoOp,
		Xor,
		Or,
		Nor,
		Equivalent,
		Invert,
		OrReverse,
		CopyInverted,
		OrInverted,
		Nand,
		Set,
	};
}

////////////////////////////////////////////////////////////////////////////////

struct GfRasterState 
{
	f32					m_fDepthBiasConstFactor			= 0.0f;
	f32					m_fDepthBiasClamp				= 0.0f;
	f32					m_fDepthBiasSlopeFactor			= 0.0f;
	f32					m_fLineWidth					= 1.0f;
	EPolygonMode::Type	m_ePolygonMode					= EPolygonMode::Fill;
	ECullMode::Type		m_eCullMode						= ECullMode::Back;
	EFrontFace::Type	m_eFrontFace					= EFrontFace::Clockwise;
	bool				m_bDepthClampEnabled			= false;
	bool				m_bRasterizerDiscardEnabled		= false;
	bool				m_bDepthBiasEnabled				= false;
};

////////////////////////////////////////////////////////////////////////////////

struct GfMultiSamplingState 
{
	EMultiSampleCount::Type m_uiSampleCount = EMultiSampleCount::Samples_1;
	bool					m_bEnabled		= false;
};

////////////////////////////////////////////////////////////////////////////////

struct GfBlendState 
{
	v4					m_vBlendConstants		= v4(0.0f);
	EBlendFactor::Type	m_eSrcColorBlendFactor	= EBlendFactor::Src_Alpha;
	EBlendFactor::Type	m_eDstColorBlendFactor	= EBlendFactor::One_Minus_Src_Alpha;
	EBlendOp::Type		m_eColorBlendOp			= EBlendOp::Add;
	EBlendFactor::Type	m_eSrcAlphaBlendFactor	= EBlendFactor::One;
	EBlendFactor::Type	m_eDstAlphaBlendFactor	= EBlendFactor::One;
	EBlendOp::Type		m_eAlphaBlendOp			= EBlendOp::Add;
	EBlendLogicOp::Type m_eBlendLogicOp			= EBlendLogicOp::Copy;
	bool				m_bEnabled				= false;
	bool				m_bLogicOpEnabled		= false;
};

////////////////////////////////////////////////////////////////////////////////

class GfVertexDeclaration 
{
public:

	struct GfVertexAttrib 
	{
		u16						m_uiOffset;	// Offset within the struct where this attrib is
		EAttributeFormat::Type	m_eType;	// Type of the attribute
	};

	GfVertexDeclaration();

	void Init(GfVertexAttrib* pAttributes, u32 uiAttribCount, u32 uiStride, EVertexInputRate::Type eRate);

	GfVertexAttrib GetAttrib(u32 uiSlot) const;

	EVertexInputRate::Type GetRate() const { return m_eRate; }

	u32 GetAttribCount() const { return m_uiAttribCount; }

	u32 GetStride() const { return m_uiVertexStride; }

	static constexpr u32 ms_uiVertexMaxAttribCount = 6;

private:

	GfVertexAttrib			m_pAttribs[ms_uiVertexMaxAttribCount];
	u32						m_uiAttribCount;
	u32						m_uiVertexStride;
	EVertexInputRate::Type	m_eRate;
};

////////////////////////////////////////////////////////////////////////////////

struct GfViewport 
{
	f32 m_fOffsetX = 0.0f;
	f32 m_fOffsetY = 0.0f;
	f32 m_fMinDepth = 0.0f;
	f32 m_fMaxDepth = 1.0f;
	f32 m_fWidth;
	f32 m_fHeight;
};

////////////////////////////////////////////////////////////////////////////////

struct GfScissor 
{
	s32 m_siOffsetX = 0;
	s32 m_siOffsetY = 0;
	s32 m_siWidth;
	s32 m_siHeight;
};

////////////////////////////////////////////////////////////////////////////////

#include GF_SOLVE_PLATFORM_INLINE(GfMaterialShared)

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIALSHARED_H__