#pragma once
////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfShaderPipeline_Platform.h
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFSHADERPIPELINE_PLATFORM_H__
#define __GFSHADERPIPELINE_PLATFORM_H__

////////////////////////////////////////////////////////////////////////////////

struct GfRenderState;

struct VkVariantData 
{
	VkPipeline m_pipeline;
	VkPipelineLayout m_layout;
};

class GfShaderPipeline_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfShaderPipeline);
public:

	VkPipeline getOrCreateGraphicsPipeline(GfVariantHash variantHash,
		const struct GfShaderPipeConfig* config,
		const class GfVertexDeclaration* vertexFormat,
		const class GfRenderPass* renderPass);

private:
	GfMap<u64, VkVariantData> m_pipelinesCache;
	GfMap<u64, VkPipelineLayout> m_layoutsCache;
};

#endif