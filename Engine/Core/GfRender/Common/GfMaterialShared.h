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
// TODO: BlendState
// TODO: Vertex description

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIALSHARED_H__