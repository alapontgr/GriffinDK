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

class GfDescriptorSetCache;

struct GfLayoutDataVK 
{
	VkPipelineLayout m_layout = VK_NULL_HANDLE;
	GfArray<GfDescriptorSetCache*, s_MAX_DESCRIPTOR_SETS> m_setLayoutCaches;
};

struct GfVariantDataVK 
{
	VkPipeline m_pipeline = VK_NULL_HANDLE;
	const GfLayoutDataVK* m_layoutData = nullptr;
};

class GfShaderPipeline_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfShaderPipeline);
public:

	const GfVariantDataVK* getOrCreateGraphicsPipeline(const GfRenderContext& ctx, 
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
	GfMap<GfVariantHash, GfVariantDataVK*> m_pipelineCache;
	GfPool<GfVariantDataVK, 16> m_pipelinePool;
	GfMap<s32, VkShaderModule> m_modulesCache;
};

class GfDescriptorSetCache 
{
public:

	GfDescriptorSetCache();

	void init(VkDescriptorSetLayout layout, const GfWeakArray<GfDescriptorBindingSlot>& setBindings);

	VkDescriptorSetLayout getLayout() const { return m_layout; }



private:
	VkDescriptorSetLayout m_layout;
	GfArray<GfDescriptorBindingSlot, s_MAX_BINDINGS_PER_SET> m_bindingsDef;
	u32 m_usedBindings;
};

class GfDescriptorSetFactoryVK
{
public:
	GfDescriptorSetFactoryVK(VkDevice device) 
		: m_device(device)	
	{
		// Use empty layout to fill gaps
		GfWeakArray<GfDescriptorBindingSlot> emptyArray(nullptr, 0);
		m_emptyDescSetLayout.init(createDescriptorSetLayout(emptyArray), emptyArray);
	}

	void shutdown();

	void tick();

	const GfLayoutDataVK* getOrCreatePipelineLayout(const GfShaderDeserializer& deserializer, const GfShaderVariantData* variantData);

	GfDescriptorSetCache* getOrCreateDescriptorSetLayoutCache(u64 hash, const GfWeakArray<GfDescriptorBindingSlot>& setBindings);

private:

	VkDescriptorSetLayout createDescriptorSetLayout(const GfWeakArray<GfDescriptorBindingSlot>& setBindings);


	VkDevice m_device;

	GfMutex m_pipelineLayoutCacheMutex;
	GfMap<u64, GfLayoutDataVK*> m_pipelineLayoutCache;
	GfPool<GfLayoutDataVK, 16> m_layoutDataPool;

	GfMutex m_descSetLayoutCacheMutex;
	GfMap<u64, GfDescriptorSetCache*> m_descSetCacheCache;
	GfPool<GfDescriptorSetCache, 64> m_descSetCachePool;
	GfDescriptorSetCache m_emptyDescSetLayout;
};

#endif