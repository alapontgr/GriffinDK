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

#include "Common/GfShaderCompiler/GfShaderCache.h"

////////////////////////////////////////////////////////////////////////////////

struct GfRenderState;
class GfRenderContext;

struct GfVariantDataVK 
{
	VkPipeline m_pipeline;
	VkPipelineLayout m_layout;
};

class GfShaderPipeline_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfShaderPipeline);
public:

	GfVariantDataVK getOrCreateGraphicsPipeline(const GfRenderContext& ctx, 
		const u64 hash,
		const GfShaderVariantData* variantData,
		const struct GfShaderPipeConfig* config,
		const class GfVertexDeclaration* vertexFormat,
		const class GfRenderPass* renderPass);

private:
	
	VkPipeline createPipeline(const GfRenderContext& ctx, 
		const GfShaderVariantData* variantData,
		const struct GfShaderPipeConfig* config,
		const class GfVertexDeclaration* vertexFormat,
		VkRenderPass renderPass, 
		VkPipelineLayout layout);

	void populateShaderStages(const GfRenderContext& ctx,
		const GfShaderVariantData* variantData,
		const u32 usedStages, 
		VkPipelineShaderStageCreateInfo* stagesToFill);

	VkShaderModule getOrCreateModule(const GfRenderContext& ctx, const GfShaderVariantData* variantData, ShaderStage::Type stage);

	GfMutex m_pipelineCacheMutex;
	GfMap<GfVariantHash, GfVariantDataVK> m_pipelineCache;
	GfMap<s32, VkShaderModule> m_modulesCache;
};

class GfDescriptorSetFactoryVK
{
public:
	GfDescriptorSetFactoryVK(VkDevice device) 
		: m_device(device)	
	{
		// Use empty layout to fill gaps
		GfWeakArray<GfDescriptorBindingSlot> emptyArray(nullptr, 0);
		m_emptyDescSetLayout = createDescriptorSetLayout(emptyArray);
	}

	void shutdown();

	void tick();

	VkPipelineLayout getOrCreatePipelineLayout(const GfShaderDeserializer& deserializer, const GfShaderVariantData* variantData);

	VkDescriptorSetLayout getOrCreateDescriptorSetLayout(u64 hash, const GfWeakArray<GfDescriptorBindingSlot>& setBindings);

	VkDescriptorSetLayout getEmptySetLayout() const { return m_emptyDescSetLayout; }

private:

	VkDescriptorSetLayout createDescriptorSetLayout(const GfWeakArray<GfDescriptorBindingSlot>& setBindings);


	VkDevice m_device;

	GfMutex m_pipelineLayoutCacheMutex;
	GfMap<u64, VkPipelineLayout> m_pipelineLayoutCache;
	GfMutex m_descSetLayoutCacheMutex;
	GfMap<u64, VkDescriptorSetLayout> m_descSetLayoutCache;
	VkDescriptorSetLayout m_emptyDescSetLayout;
};

#endif