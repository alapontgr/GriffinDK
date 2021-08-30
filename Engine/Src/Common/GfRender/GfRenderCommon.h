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

////////////////////////////////////////////////////////////////////////////////

namespace EParamaterSlotType
{
	enum Type : u32
	{
		Sampler = 0,
		CombinedTextureSampler,
		Texture,
		SamplerTexture, // Sampler + Texture
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
		Count,
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EShaderStageFlags
{
	enum FlagBit : u32
	{
		Vertex = 1 << EShaderStage::Vertex,
		TesellationControl = 1 << EShaderStage::TesellationControl,
		TesellationEval = 1 << EShaderStage::TesellationEval,
		Geometry = 1 << EShaderStage::Geometry,
		Fragment = 1 << EShaderStage::Fragment,
		Compute = 1 << EShaderStage::Compute,
		// All the stages within the graphics pipeline
		AllGraphics = Vertex | TesellationControl | TesellationEval | Geometry | Fragment,
	};
}
using GfShaderAccessMask = GfBitMask<u32>;

static constexpr bool isGraphics(EShaderStage::Type stage) 
{
	return ((1 << stage) & EShaderStageFlags::AllGraphics) != 0;
}

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

		Invalid = (~0u),
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
		////////////////////////////////////////////////////////////////////////////////
		RequiredBits = 4 // Precision needed to represent this type as a bit-field
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EPolygonMode
{
	enum Type : u32
	{
		Fill = 0,
		Line,
		Point,
		////////////////////////////////////////////////////////////////////////////////
		RequiredBits = 2 // Precision needed to represent this type as a bit-field
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace ECullMode
{
	enum Type : u32
	{
		None,
		Front,
		Back,
		FrontBack,
		////////////////////////////////////////////////////////////////////////////////
		RequiredBits = 2 // Precision needed to represent this type as a bit-field
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EFrontFace
{
	enum Type : u32
	{
		CounterClockwise = 0,
		Clockwise,
		////////////////////////////////////////////////////////////////////////////////
		RequiredBits = 1 // Precision needed to represent this type as a bit-field
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EMultiSampleCount
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
		RequiredBits = 3 // Precision needed to represent this type as a bit-field
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EBlendFactor
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
		RequiredBits = 5 // Precision needed to represent this type as a bit-field
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EBlendOp
{
	enum Type : u32
	{
		Add = 0,
		Subtract,
		Reverse_Subtract,
		Min,
		Max,
		////////////////////////////////////////////////////////////////////////////////
		RequiredBits = 3 // Precision needed to represent this type as a bit-field
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace EAttributeFormat
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
		RequiredBits = 4 // Precision needed to represent this type as a bit-field
	};
}

////////////////////////////////////////////////////////////////////////////////

/*
See https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkLogicOp.html
*/
namespace EBlendLogicOp
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
		RequiredBits = 4 // Precision needed to represent this type as a bit-field
	};
}

////////////////////////////////////////////////////////////////////////////////

struct GfRasterState
{
	GfRasterState() 
		: m_fDepthBiasConstFactor(0.0f)
		, m_fDepthBiasClamp(0.0f)
		, m_fDepthBiasSlopeFactor(0.0f)
		, m_fLineWidth(1.0f)
		, m_ePolygonMode(EPolygonMode::Fill)
		, m_eCullMode(ECullMode::Back)
		, m_eFrontFace(EFrontFace::CounterClockwise)
		, m_bDepthClampEnabled(GF_FALSE)
		, m_bRasterizerDiscardEnabled(GF_FALSE)
		, m_bDepthBiasEnabled(GF_FALSE)
		{}

	f32	m_fDepthBiasConstFactor;
	f32	m_fDepthBiasClamp;
	f32	m_fDepthBiasSlopeFactor;
	f32	m_fLineWidth;
	u32	m_ePolygonMode : EPolygonMode::RequiredBits;
	u32	m_eCullMode : ECullMode::RequiredBits;
	u32	m_eFrontFace : EFrontFace::RequiredBits;
	u32	m_bDepthClampEnabled : 1;
	u32	m_bRasterizerDiscardEnabled : 1;
	u32	m_bDepthBiasEnabled : 1;
};

////////////////////////////////////////////////////////////////////////////////

struct GfMultiSamplingState
{
	GfMultiSamplingState() 
		: m_uiSampleCount(EMultiSampleCount::Samples_1)
		, m_bEnabled(GF_FALSE)
	{}

	EMultiSampleCount::Type m_uiSampleCount : EMultiSampleCount::RequiredBits;
	u32						m_bEnabled : 1;
};

////////////////////////////////////////////////////////////////////////////////

struct GfBlendState
{
	GfBlendState() 
		: m_vBlendConstants(0.0f)
		, m_eSrcColorBlendFactor(EBlendFactor::Src_Alpha)
		, m_eDstColorBlendFactor(EBlendFactor::One_Minus_Src_Alpha)
		, m_eColorBlendOp(EBlendOp::Add)
		, m_eSrcAlphaBlendFactor(EBlendFactor::One)
		, m_eDstAlphaBlendFactor(EBlendFactor::One)
		, m_eAlphaBlendOp(EBlendOp::Add)
		, m_eBlendLogicOp(EBlendLogicOp::Copy)
		, m_bEnabled(GF_FALSE)
		, m_bLogicOpEnabled(GF_FALSE)
	{}

	v4	m_vBlendConstants;
	u32 m_eSrcColorBlendFactor : EBlendFactor::RequiredBits;
	u32 m_eDstColorBlendFactor : EBlendFactor::RequiredBits;
	u32 m_eColorBlendOp : EBlendOp::RequiredBits;
	u32 m_eSrcAlphaBlendFactor : EBlendFactor::RequiredBits;
	u32 m_eDstAlphaBlendFactor : EBlendFactor::RequiredBits;
	u32 m_eAlphaBlendOp : EBlendOp::RequiredBits;
	u32 m_eBlendLogicOp : EBlendLogicOp::RequiredBits;
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

////////////////////////////////////////////////////////////////////////////////

// This represents a binding slot in a Descriptor. Used to represent the layout of uniforms.
struct alignas(4) GfDescriptorBindingSlot 
{
	u32 m_stageFlags : 12; // Stages accessing the resource. See EShaderStageFlags
	u32 m_descriptorType : 4; // See EParamaterSlotType
	u32 m_bindingSlot : 16; //
	u32 m_arraySize; // If array, the number of entries.
};
static_assert(EParamaterSlotType::Count <= (1<<4), "GfDescriptorBindingSlot::m_descriptorType overflows");
static_assert(EShaderStage::Count <= 12, "GfDescriptorBindingSlot::m_stageFlags overflows");

////////////////////////////////////////////////////////////////////////////////

static constexpr u32 s_MAX_DESCRIPTOR_SETS = 8;
static constexpr u32 s_MAX_BINDINGS_PER_SET = 16;

////////////////////////////////////////////////////////////////////////////////

#include GF_SOLVE_GFX_API_PATH(GfRender/GfRenderCommon_Platform.inl)

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERCOMMON_H__