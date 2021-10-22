////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/01/14
//	File: 	GfRenderCommon.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFRENDERCOMMON_H__
#define __GFRENDERCOMMON_H__
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfMemory/GfStackAllocator.h"
#include "Common/GfCore/GfBitMask.h"

// Multi-threaded stack allocator. Each thread holds an instance of a Stack allocator for temporal memory management
using GfFrameMTStackAlloc = GfPerThreadStackAllocator<GfDefaultAllocator, GF_KB(64)>;

// Use this for temporary allocations
using GfFrameTmpAllocator = GfSingleton<GfLinearAllocator>;

////////////////////////////////////////////////////////////////////////////////

namespace ParamaterSlotType
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
		InputAttachment,
		////////////////////////////////////////////////////////////////////////////////
		Count,
		Invalid = (~0u),
		RequiredBits = 4 // Precision needed to represent this type as a bit-field
	};
	static_assert(ParamaterSlotType::Count <= (1 << ParamaterSlotType::RequiredBits), "Invalid requiredBits");
}

////////////////////////////////////////////////////////////////////////////////

namespace ShaderStage
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
		Count,
		RequiredBits = 3
	};
	static_assert(ShaderStage::Count <= (1 << ShaderStage::RequiredBits), "Invalid requiredBits");
}

////////////////////////////////////////////////////////////////////////////////

namespace ShaderStageFlags
{
	enum FlagBit : u32
	{
		Vertex = 1 << ShaderStage::Vertex,
		TesellationControl = 1 << ShaderStage::TesellationControl,
		TesellationEval = 1 << ShaderStage::TesellationEval,
		Geometry = 1 << ShaderStage::Geometry,
		Fragment = 1 << ShaderStage::Fragment,
		Compute = 1 << ShaderStage::Compute,
		// All the stages within the graphics pipeline
		AllGraphics = Vertex | TesellationControl | TesellationEval | Geometry | Fragment,
	};
}
using GfShaderAccessMask = GfBitMask<u32>;

static constexpr bool isGraphics(ShaderStage::Type stage) 
{
	return ((1 << stage) & ShaderStageFlags::AllGraphics) != 0;
}

////////////////////////////////////////////////////////////////////////////////

namespace VertexInputRate
{
	enum Type : u32
	{
		PerVertex = 0,
		PerInstance,
		////////////////////////////////////////////////////////////////////////////////
		RequiredBits = 1 // Precision needed to represent this type as a bit-field
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace MaterialParamRate
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

		Invalid = (~0u),
	};
}
static_assert(MaterialParamRate::Count <= MaterialParamRate::MaxBoundSets, "Invalid count of rates");

////////////////////////////////////////////////////////////////////////////////

namespace PrimitiveTopology
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
		////////////////////////////////////////////////////////////////////////////////
		Count,
		RequiredBits = 4 // Precision needed to represent this type as a bit-field
	};
	static_assert(PrimitiveTopology::Count <= (1 << PrimitiveTopology::RequiredBits), "Invalid requiredBits");
}

////////////////////////////////////////////////////////////////////////////////

namespace PolygonMode
{
	enum Type : u32
	{
		Fill = 0,
		Line,
		Point,
		////////////////////////////////////////////////////////////////////////////////
		Count,
		RequiredBits = 2 // Precision needed to represent this type as a bit-field
	};
	static_assert(PolygonMode::Count <= (1 << PolygonMode::RequiredBits), "Invalid requiredBits");
}

////////////////////////////////////////////////////////////////////////////////

namespace CullMode
{
	enum Type : u32
	{
		None,
		Front,
		Back,
		FrontBack,
		////////////////////////////////////////////////////////////////////////////////
		Count,
		RequiredBits = 2 // Precision needed to represent this type as a bit-field
	};
	static_assert(CullMode::Count <= (1 << CullMode::RequiredBits), "Invalid requiredBits");
}

////////////////////////////////////////////////////////////////////////////////

namespace FrontFace
{
	enum Type : u32
	{
		CounterClockwise = 0,
		Clockwise,
		////////////////////////////////////////////////////////////////////////////////
		Count,
		RequiredBits = 1 // Precision needed to represent this type as a bit-field
	};
	static_assert(FrontFace::Count <= (1 << FrontFace::RequiredBits), "Invalid requiredBits");
}

////////////////////////////////////////////////////////////////////////////////

namespace MultiSampleCount
{
	enum Type : u32
	{
		Samples_1 = 0,
		Samples_2,
		Samples_4,
		Samples_8,
		Samples_16,
		Samples_32,
		Samples_64,
		////////////////////////////////////////////////////////////////////////////////
		Count,
		RequiredBits = 3 // Precision needed to represent this type as a bit-field
	};
	static_assert(MultiSampleCount::Count <= (1 << MultiSampleCount::RequiredBits), "Invalid requiredBits");
}

////////////////////////////////////////////////////////////////////////////////

namespace BlendFactor
{
	enum Type : u32
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
		////////////////////////////////////////////////////////////////////////////////
		Count,
		RequiredBits = 5 // Precision needed to represent this type as a bit-field
	};
	static_assert(BlendFactor::Count <= (1 << BlendFactor::RequiredBits), "Invalid requiredBits");
}

////////////////////////////////////////////////////////////////////////////////

namespace BlendOp
{
	enum Type : u32
	{
		Add = 0,
		Subtract,
		Reverse_Subtract,
		Min,
		Max,
		////////////////////////////////////////////////////////////////////////////////
		Count,
		RequiredBits = 3 // Precision needed to represent this type as a bit-field
	};
	static_assert(BlendOp::Count <= (1 << BlendOp::RequiredBits), "Invalid requiredBits");
}

////////////////////////////////////////////////////////////////////////////////

namespace AttributeFormat
{
	enum Type : u32
	{
		SFloat = 0,	// R32
		SFloat2,	// R32G32
		SFloat3,	// R32G32B32
		SFloat4,	// R32G32B32A32

		SInt,		// R32
		SInt2,		// R32G32
		SInt3,		// R32G32B32
		SInt4,		// R32G32B32A32

		UInt,		// R32
		UInt2,		// R32G32
		UInt3,		// R32G32B32
		UInt4,		// R32G32B32A32

		////////////////////////////////////////////////////////////////////////////////
		Count,
		RequiredBits = 4 // Precision needed to represent this type as a bit-field
	};
	static_assert(AttributeFormat::Count <= (1 << AttributeFormat::RequiredBits), "Invalid requiredBits");
}


////////////////////////////////////////////////////////////////////////////////

/*
See https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkLogicOp.html
*/
namespace BlendLogicOp
{
	enum Type : u32
	{
		Clear = 0,
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
		////////////////////////////////////////////////////////////////////////////////
		Count,
		RequiredBits = 4 // Precision needed to represent this type as a bit-field
	};
	static_assert(BlendLogicOp::Count <= (1 << BlendLogicOp::RequiredBits), "Invalid requiredBits");
}

namespace CompareOp 
{
	enum Type : u32 
	{
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always,

		Count,
		RequiredBits = 3
	};
	static_assert(CompareOp::Count <= (1 << CompareOp::RequiredBits), "Invalid requiredBits");
}

namespace StencilOp 
{
	enum Type : u32 
	{
		Keep,
		Zero,
		Replace,
		IncrementClamp,
		DecrementClamp,
		Invert,
		IncrementWarp,
		DecrementWarp,

		Count,
		RequiredBits = 3
	};
	static_assert(StencilOp::Count <= (1 << StencilOp::RequiredBits), "Invalid requiredBits");
}

namespace StencilFace 
{
	enum Type : u32 
	{
		Front = 0,
		Back,
		Both,

		Count,
		RequiredBits = 2
	};
	static_assert(StencilFace::Count <= (1 << StencilFace::RequiredBits), "Invalid requiredBits");
}

////////////////////////////////////////////////////////////////////////////////

struct GfRasterState
{
	GfRasterState() 
		: m_depthBiasConstFactor(0.0f)
		, m_depthBiasClamp(0.0f)
		, m_depthBiasSlopeFactor(0.0f)
		, m_lineWidth(1.0f)
		, m_polygonMode(PolygonMode::Fill)
		, m_cullMode(CullMode::Back)
		, m_frontFace(FrontFace::CounterClockwise)
		, m_depthClampEnabled(GF_FALSE)
		, m_rasterizerDiscardEnabled(GF_FALSE)
		, m_depthBiasEnabled(GF_FALSE)
		{}

	f32	m_depthBiasConstFactor;
	f32	m_depthBiasClamp;
	f32	m_depthBiasSlopeFactor;
	f32	m_lineWidth;
	PolygonMode::Type m_polygonMode : PolygonMode::RequiredBits;
	CullMode::Type	m_cullMode : CullMode::RequiredBits;
	FrontFace::Type	m_frontFace : FrontFace::RequiredBits;
	u32	m_depthClampEnabled : 1;
	u32	m_rasterizerDiscardEnabled : 1;
	u32	m_depthBiasEnabled : 1;
};

////////////////////////////////////////////////////////////////////////////////

struct GfMultiSamplingState
{
	GfMultiSamplingState() 
		: m_uiSampleCount(MultiSampleCount::Samples_1)
		, m_bEnabled(GF_FALSE)
	{}

	MultiSampleCount::Type m_uiSampleCount : MultiSampleCount::RequiredBits;
	u32						m_bEnabled : 1;
};

////////////////////////////////////////////////////////////////////////////////

struct GfBlendState
{
	GfBlendState() 
		: m_blendConstants(0.0f)
		, m_srcColorBlendFactor(BlendFactor::Src_Alpha)
		, m_dstColorBlendFactor(BlendFactor::One_Minus_Src_Alpha)
		, m_colorBlendOp(BlendOp::Add)
		, m_srcAlphaBlendFactor(BlendFactor::One)
		, m_dstAlphaBlendFactor(BlendFactor::One)
		, m_alphaBlendOp(BlendOp::Add)
		, m_blendLogicOp(BlendLogicOp::Copy)
		, m_bEnabled(GF_FALSE)
		, m_bLogicOpEnabled(GF_FALSE)
	{}

	v4	m_blendConstants;
	BlendFactor::Type m_srcColorBlendFactor : BlendFactor::RequiredBits;
	BlendFactor::Type m_dstColorBlendFactor : BlendFactor::RequiredBits;
	BlendOp::Type m_colorBlendOp : BlendOp::RequiredBits;
	BlendFactor::Type m_srcAlphaBlendFactor : BlendFactor::RequiredBits;
	BlendFactor::Type m_dstAlphaBlendFactor : BlendFactor::RequiredBits;
	BlendOp::Type m_alphaBlendOp : BlendOp::RequiredBits;
	BlendLogicOp::Type m_blendLogicOp : BlendLogicOp::RequiredBits;
	u32	m_bEnabled : 1;
	u32	m_bLogicOpEnabled : 1;
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


struct GfDepthState
{
	struct GfStencilState 
	{
		GfStencilState() 
			: m_failOp(StencilOp::Keep)
			, m_passOp(StencilOp::Keep)
			, m_depthFailOp(StencilOp::Keep)
			, m_compareOp(CompareOp::Never)
		{}

		StencilOp::Type m_failOp : StencilOp::RequiredBits;
		StencilOp::Type m_passOp : StencilOp::RequiredBits;
		StencilOp::Type m_depthFailOp : StencilOp::RequiredBits;
		CompareOp::Type m_compareOp : CompareOp::RequiredBits;
	};

	GfDepthState()
		: m_depthTestEnabled(GF_FALSE)
		, m_depthWriteEnabled(GF_FALSE)
		, m_stencilEnabled(GF_FALSE) 
		, m_depthCompareOp(CompareOp::Always){}
	
	u32 m_depthTestEnabled : 1;
	u32 m_depthWriteEnabled : 1;
	u32 m_stencilEnabled : 1;
	CompareOp::Type m_depthCompareOp : CompareOp::RequiredBits;
	GfStencilState m_stencilFront;
	GfStencilState m_stencilBack;
};

////////////////////////////////////////////////////////////////////////////////

// This represents a binding slot in a Descriptor. Used to represent the layout of uniforms.
struct alignas(4) GfDescriptorBindingSlot 
{
	u32 m_stageFlags : 12; // Stages accessing the resource. See ShaderStageFlags
	ParamaterSlotType::Type m_descriptorType : ParamaterSlotType::RequiredBits; // See ParamaterSlotType
	u32 m_arraySize : 16; // If array, the number of entries.
};
static_assert(ShaderStage::Count <= 12, "GfDescriptorBindingSlot::m_stageFlags overflows");

////////////////////////////////////////////////////////////////////////////////

static constexpr u32 s_MAX_DESCRIPTOR_SETS = 8;
static constexpr u32 s_MAX_BINDINGS_PER_SET = 16;

////////////////////////////////////////////////////////////////////////////////

#include GF_SOLVE_GFX_API_PATH(GfRender/GfRenderCommon_Platform.inl)

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERCOMMON_H__