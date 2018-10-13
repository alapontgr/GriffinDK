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
	};
}
using GfShaderAccessMask = GfBitMask<u32>;

////////////////////////////////////////////////////////////////////////////////

namespace EMaterialParamRate 
{
	enum Type : u32 
	{
		PerFrame = 0,	// Common to all the objects of a frame
		PerRenderPass,	// Resources local to a RenderPass
		PerMaterial,	// Shared within all the instances of the same material
		PerInstance,	// Local configuration of each instance
		////////////////////////////////////////////////////////////////////////////////
		Count,
		Invalid = 0xffffffff
	};
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIALSHARED_H__