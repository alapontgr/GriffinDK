////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/24
//	File: 	GfGraphicResourcesShared.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFGRAPHICRESOURCESSHARED_H__
#define __GFGRAPHICRESOURCESSHARED_H__
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfCoreMinimal.h"

////////////////////////////////////////////////////////////////////////////////

namespace GfTextureFormat 
{
	// Ref: https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkFormat.html
	enum Type : u16 
	{
		Undefined = 0,
		// 2-comp, 8b
		R4G4_UNorm,	
		// 4-comp, 16b
		R4G4B4A4_UNorm,
		B4G4R4A4_UNorm,
		// 3-comp, 16b
		R5G6B5_UNorm,
		B5G6R5_UNorm,
		// 4-comp, 16b
		R5G5B5A1_UNorm,
		B5G5R5A1_UNorm,
		A1R5G5B5_UNorm,
		// 8b
		R8_UNorm,	
		R8_SNorm,	
		R8_UScaled,	
		R8_SScaled,	
		R8_UInt,
		R8_SInt,
		R8_SRGB,
		// 2-comp, 16b
		R8G8_UNorm,
		R8G8_SNorm,
		R8G8_UScaled,
		R8G8_SScaled,
		R8G8_UInt,
		R8G8_SInt,
		R8G8_SRGB,
		// 3-comp, 24b
		R8G8B8_UNorm,
		R8G8B8_SNorm,
		R8G8B8_UScaled,
		R8G8B8_SScaled,
		R8G8B8_UInt,
		R8G8B8_SInt,
		R8G8B8_SRGB,
		B8G8R8_UNorm,
		B8G8R8_SNorm,
		B8G8R8_UScaled,
		B8G8R8_SScaled,
		B8G8R8_UInt,
		B8G8R8_SInt,
		B8G8R8_SRGB,
		// 4-comp, 32b
		R8G8B8A8_UNorm,				
		R8G8B8A8_SNorm,
		R8G8B8A8_UScaled,
		R8G8B8A8_SScaled,
		R8G8B8A8_UINT,
		R8G8B8A8_SINT,
		R8G8B8A8_SRGB,
		B8G8R8A8_UNorm,
		B8G8R8A8_SNORM,
		B8G8R8A8_UScaled,
		B8G8R8A8_SScaled,
		B8G8R8A8_UINT,
		B8G8R8A8_SINT,
		B8G8R8A8_SRGB,
		A8B8G8R8_UNorm_Pack32,
		A8B8G8R8_SNorm_Pack32,
		A8B8G8R8_UScaled_Pack32,
		A8B8G8R8_SScaled_Pack32,
		A8B8G8R8_UINT_Pack32,
		A8B8G8R8_SInt_Pack32,
		A8B8G8R8_SRGB_Pack32,
		// 4-comp, 32b (Special packing)
		A2R10G10B10_UNorm_Pack32,
		A2R10G10B10_SNorm_Pack32,
		A2R10G10B10_UScaled_Pack32,
		A2R10G10B10_SScaled_Pack32,
		A2R10G10B10_UInt_Pack32,
		A2R10G10B10_SInt_Pack32,
		A2B10G10R10_UNorm_Pack32,
		A2B10G10R10_SNorm_Pack32,
		A2B10G10R10_UScaled_Pack32,
		A2B10G10R10_SScaled_Pack32,
		A2B10G10R10_UInt_Pack32,
		A2B10G10R10_SInt_Pack32,
		// 1-comp, 16b
		R16_UNorm,
		R16_SNorm,
		R16_UScaled,
		R16_SScaled,
		R16_UInt,
		R16_SInt,
		R16_SFloat,
		// 2-comp, 32b
		R16G16_UNorm,
		R16G16_SNorm,
		R16G16_UScaled,
		R16G16_SScaled,
		R16G16_UInt,
		R16G16_SInt,
		R16G16_SFloat,
		// 3-comp, 48b
		R16G16B16_UNorm,
		R16G16B16_SNorm,
		R16G16B16_UScaled,
		R16G16B16_SScaled,
		R16G16B16_UInt,
		R16G16B16_SInt,
		R16G16B16_SFloat,
		// 4-comp, 64b
		R16G16B16A16_UNorm,
		R16G16B16A16_SNorm,
		R16G16B16A16_UScaled,
		R16G16B16A16_SScaled,
		R16G16B16A16_UInt,
		R16G16B16A16_SInt,
		R16G16B16A16_SFloat,
		// 1-comp, 32b
		R32_UInt,
		R32_SInt,
		R32_SFloat,
		// 2-comp, 64b
		R32G32_UInt,
		R32G32_SInt,
		R32G32_SFloat,
		// 3-comp, 86b
		R32G32B32_UInt,
		R32G32B32_SInt,
		R32G32B32_SFloat,
		// 4-comp, 128b
		R32G32B32A32_UInt,
		R32G32B32A32_SInt,
		R32G32B32A32_SFloat,
		// 1-comp, 64b
		R64_UInt,
		R64_SInt,
		R64_SFloat,
		// 2-comp, 128b
		R64G64_UInt,
		R64G64_SInt,
		R64G64_SFloat,
		// 3-comp, 192b
		R64G64B64_UInt,
		R64G64B64_SInt,
		R64G64B64_SFloat,
		// 4-comp, 256b
		R64G64B64A64_UInt,
		R64G64B64A64_SInt,
		R64G64B64A64_SFloat,
		
		// (special packing)
		B10G11R11_UFloat_Pack32,
		E5B9G9R9_UFloat_Pack32,

		// Depth formats
		D16_UNorm,
		X8_D24_UNorm_Pack32,
		D32_SFloat,

		// Stencil formats
		S8_UInt,

		// Depth-Stencil formats
		D16_UNorm_S8_UInt,
		D24_UNorm_S8_UInt,
		D32_SFloat_S8_UInt,

		// Block compression formats
		BC1_RGB_UNorm_Block,
		BC1_RGB_SRGB_Block,
		BC1_RGBA_UNorm_Block,
		BC1_RGBA_SRGB_Block,
		BC2_UNorm_Block,
		BC2_SRGB_Block,
		BC3_UNorm_Block,
		BC3_SRGB_Block,
		BC4_UNorm_Block,
		BC4_SNorm_Block,
		BC5_UNorm_Block,
		BC5_SNorm_Block,
		BC6H_UFLOAT_Block,
		BC6H_SFLOAT_Block,
		BC7_UNorm_Block,
		BC7_SRGB_Block,

		////////////////////////////////////////////////////////////////////////////////
		COUNT
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace ETextureUsageShift 
{
	// Ref: https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkImageUsageFlagBits.html
	enum Type : u16 
	{
		Transfer_Src = 0,
		Transfer_Dst,
		Sampled,
		Storage,
		ColorAttachment,
		DepthStencilAttachment,
		TransientAttachment,
		InputAttachment,

		////////////////////////////////////////////////////////////////////////////////
		COUNT
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace ETextureViewType
{
	enum Type : u32
	{
		View1D = 0,
		View2D,
		View3D,
		ViewCube,
		View1DArray,
		View2DArray,
		ViewCubeArray,
		////////////////////////////////////////////////////////////////////////
		COUNT
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace ETexFilter
{
	enum Type : u32
	{
		Nearest = 0,
		Linear,
		Cubic
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace ESamplerMipMapMode
{
	enum Type : u32
	{
		Nearest = 0,
		Linear
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace ETexAddressMode
{
	enum Type : u32
	{
		Repeat = 0,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
		MirrorClampToEdge
	};
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool isDepthFormat(GfTextureFormat::Type format) 
{
	return (format & (GfTextureFormat::D16_UNorm | 
		GfTextureFormat::D16_UNorm_S8_UInt |
		GfTextureFormat::D24_UNorm_S8_UInt |
		GfTextureFormat::D32_SFloat | 
		GfTextureFormat::D32_SFloat_S8_UInt)) != 0;
}

GF_FORCEINLINE bool isStencilFormat(GfTextureFormat::Type format) 
{
	return (format & (GfTextureFormat::D16_UNorm_S8_UInt |
		GfTextureFormat::D24_UNorm_S8_UInt |
		GfTextureFormat::S8_UInt | 
		GfTextureFormat::D32_SFloat_S8_UInt)) != 0;
}

////////////////////////////////////////////////////////////////////////////////

#include GF_SOLVE_GFX_API_PATH(GfRender/GraphicResources/GfGraphicResourcesShared_Platform.inl)

////////////////////////////////////////////////////////////////////////////////
#endif // __GFGRAPHICRESOURCESSHARED_H__