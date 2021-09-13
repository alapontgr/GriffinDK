////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/26
//	File: 	GfMaterial_Platform.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfRender/GfShaderPipeline.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfShaderPipeline)
{

}

VkPipeline GfShaderPipeline_Platform::getOrCreateGraphicsPipeline(GfVariantHash variantHash,
		const GfShaderPipeConfig* config,
		const GfVertexDeclaration* vertexFormat,
		const GfRenderPass* renderPass)
{


	return VK_NULL_HANDLE;
}
