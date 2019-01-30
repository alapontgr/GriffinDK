////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/01/28
//	File: 	GfMaterialShared_Platform.inl
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMATERIALSHARED_PLATFORM_INL__
#define __GFMATERIALSHARED_PLATFORM_INL__
////////////////////////////////////////////////////////////////////////////////

#include "GfRender/Common/GfGraphicsSDK.h"

////////////////////////////////////////////////////////////////////////////////

static const VkDescriptorType g_pDescriptorTypeConverter[] =
{
	VK_DESCRIPTOR_TYPE_SAMPLER,
	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
	VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
	VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
	VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
};

////////////////////////////////////////////////////////////////////////////////

static const VkShaderStageFlags g_pShaderStageFlagsConverter[] =
{
	VK_SHADER_STAGE_VERTEX_BIT,
	VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
	VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
	VK_SHADER_STAGE_GEOMETRY_BIT,
	VK_SHADER_STAGE_FRAGMENT_BIT,
	VK_SHADER_STAGE_COMPUTE_BIT,
};

////////////////////////////////////////////////////////////////////////////////

static const VkPrimitiveTopology g_pTopologyConverter[] = 
{
	VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
	VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
	VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
	VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
	VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
	VK_PRIMITIVE_TOPOLOGY_PATCH_LIST,
};

////////////////////////////////////////////////////////////////////////////////

static const VkPolygonMode g_pPolygonModeConverter[] = 
{
	VK_POLYGON_MODE_FILL,
	VK_POLYGON_MODE_LINE,
	VK_POLYGON_MODE_POINT,
};

////////////////////////////////////////////////////////////////////////////////

static const VkCullModeFlagBits  g_pCullModeConverter[] = 
{
	VK_CULL_MODE_NONE,
	VK_CULL_MODE_FRONT_BIT,
	VK_CULL_MODE_BACK_BIT,
	VK_CULL_MODE_FRONT_AND_BACK,
};

////////////////////////////////////////////////////////////////////////////////

static const VkFrontFace g_pFrontFaceConverter[] = 
{
	VK_FRONT_FACE_COUNTER_CLOCKWISE,
	VK_FRONT_FACE_CLOCKWISE,
};

////////////////////////////////////////////////////////////////////////////////

static const VkSampleCountFlagBits  g_pSampleCountConverter[] = 
{
	VK_SAMPLE_COUNT_1_BIT,
	VK_SAMPLE_COUNT_2_BIT,
	VK_SAMPLE_COUNT_4_BIT,
	VK_SAMPLE_COUNT_8_BIT,
	VK_SAMPLE_COUNT_16_BIT,
	VK_SAMPLE_COUNT_32_BIT,
	VK_SAMPLE_COUNT_64_BIT,
};

////////////////////////////////////////////////////////////////////////////////

static const VkBlendFactor g_pBlendFactorConverter[] = 
{
	VK_BLEND_FACTOR_ZERO,
	VK_BLEND_FACTOR_ONE,
	VK_BLEND_FACTOR_SRC_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
	VK_BLEND_FACTOR_DST_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
	VK_BLEND_FACTOR_SRC_ALPHA,
	VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	VK_BLEND_FACTOR_DST_ALPHA,
	VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
	VK_BLEND_FACTOR_CONSTANT_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
	VK_BLEND_FACTOR_CONSTANT_ALPHA,
	VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
	VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
	VK_BLEND_FACTOR_SRC1_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
	VK_BLEND_FACTOR_SRC1_ALPHA,
	VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA,
};

////////////////////////////////////////////////////////////////////////////////

static const VkBlendOp g_pBlendOpConverter[] = 
{
	VK_BLEND_OP_ADD,
	VK_BLEND_OP_SUBTRACT,
	VK_BLEND_OP_REVERSE_SUBTRACT,
	VK_BLEND_OP_MIN,
	VK_BLEND_OP_MAX,
};

////////////////////////////////////////////////////////////////////////////////

static const VkFormat g_pFormatConverter[] = 
{
	VK_FORMAT_R32_SFLOAT,
	VK_FORMAT_R32G32_SFLOAT,
	VK_FORMAT_R32G32B32_SFLOAT,
	VK_FORMAT_R32G32B32A32_SFLOAT,

	VK_FORMAT_R32_SINT,
	VK_FORMAT_R32G32_SINT,
	VK_FORMAT_R32G32B32_SINT,
	VK_FORMAT_R32G32B32A32_SINT,

	VK_FORMAT_R32_UINT,
	VK_FORMAT_R32G32_UINT,
	VK_FORMAT_R32G32B32_UINT,
	VK_FORMAT_R32G32B32A32_UINT,
};

////////////////////////////////////////////////////////////////////////////////

static const VkVertexInputRate g_pInputRate[] = 
{
	VK_VERTEX_INPUT_RATE_VERTEX,
	VK_VERTEX_INPUT_RATE_INSTANCE,
};

////////////////////////////////////////////////////////////////////////////////

// Direct map for the moment
static inline VkDescriptorType ConvertDescriptorType(EParamaterSlotType::Type eType)
{
	return g_pDescriptorTypeConverter[eType];
}

////////////////////////////////////////////////////////////////////////////////

static inline VkShaderStageFlags ConvertShaderStageFlags(GfShaderAccessMask kStages)
{
	VkShaderStageFlags uiResult(0);
	for (u32 i = 0; i < EShaderStageFlags::COUNT; ++i)
	{
		if ((kStages & (1 << i)) != 0)
		{
			uiResult |= g_pShaderStageFlagsConverter[i];
		}
	}
	return uiResult;
}

////////////////////////////////////////////////////////////////////////////////

static inline VkPrimitiveTopology ConvertTopology(EPrimitiveTopology::Type eTopology) 
{
	return g_pTopologyConverter[eTopology];
}

////////////////////////////////////////////////////////////////////////////////

static inline VkPolygonMode ConvertPolygonMode(EPolygonMode::Type ePolygonMode) 
{
	return g_pPolygonModeConverter[ePolygonMode];
}

////////////////////////////////////////////////////////////////////////////////

static inline VkCullModeFlagBits ConvertCullMode(ECullMode::Type eCullMode) 
{
	return g_pCullModeConverter[eCullMode];
}

////////////////////////////////////////////////////////////////////////////////

static inline VkFrontFace ConvertFrontFace(EFrontFace::Type eFrontFace) 
{
	return g_pFrontFaceConverter[eFrontFace];
}

////////////////////////////////////////////////////////////////////////////////

static inline VkSampleCountFlagBits ConvertSampleCount(EMultiSampleCount::Type eSampleCount) 
{
	return g_pSampleCountConverter[eSampleCount];
}

////////////////////////////////////////////////////////////////////////////////

static inline VkBlendFactor ConvertBlendFactor(EBlendFactor::Type eBlendFactor) 
{
	return g_pBlendFactorConverter[eBlendFactor];
}

////////////////////////////////////////////////////////////////////////////////

static inline VkBlendOp ConvertBlendOp(EBlendOp::Type eBlendOp) 
{
	return g_pBlendOpConverter[eBlendOp];
}

////////////////////////////////////////////////////////////////////////////////

static inline VkFormat ConvertAttributeFormat(EAttributeFormat::Type eFormat) 
{
	return g_pFormatConverter[eFormat];
}

////////////////////////////////////////////////////////////////////////////////

static inline VkVertexInputRate ConvertInputRate(EVertexInputRate::Type eInputRate) 
{
	return g_pInputRate[eInputRate];
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIALSHARED_PLATFORM_INL__