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
struct GfResourceBindingExt;

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

	struct DescSetEntry : public GfIntrusiveList<DescSetEntry>::GfListEntry
	{
		VkDescriptorSet m_set = VK_NULL_HANDLE;
		u32 m_lastUsedFrame = 0;
		u64 m_hash = 0;
	};

	GfDescriptorSetCache();

	void init(VkDescriptorSetLayout layout, const GfWeakArray<GfDescriptorBindingSlot>& setBindings);

	VkDescriptorSetLayout getLayout() const { return m_layout; }

	VkDescriptorSet getOrCreateDescSet(
		const GfRenderContext& ctx,
		GfLinearAllocator& alloc,
		const GfWeakArray<GfDescriptorBindingSlot>& setBindings, 
		const GfWeakArray<GfResourceBindingExt>& resourceBindings);

private:

	u64 computeBindingsHash(
		GfLinearAllocator& alloc,
		const GfWeakArray<GfDescriptorBindingSlot>& setBindings, 
		const GfWeakArray<GfResourceBindingExt>& resourceBindings) const;

	VkDescriptorSet createDescSet(
		const GfRenderContext& ctx,
		const GfWeakArray<GfDescriptorBindingSlot>& setBindings);

	void updateDescSet(
		const GfRenderContext& ctx,
		GfLinearAllocator& alloc,
		const VkDescriptorSet set,
		const GfWeakArray<GfDescriptorBindingSlot>& setBindings,
		const GfWeakArray<GfResourceBindingExt>& resourceBindings) const;

	VkDescriptorPool getPoolToAllocateFrom(const GfRenderContext& ctx);

	static constexpr u32 ms_descSetBufferingFrames = 100;
	static constexpr u32 ms_descSetCountInPool = 64;

	VkDescriptorSetLayout m_layout;
	GfArray<u32, GfParameterSlotType::Count> m_perResourceTypeCount;
	u32 m_usedBindings;
	u32 m_reqImageWrites;
	u32 m_reqBufferWrites;

	// Caching
	GfMap<u64, DescSetEntry*> m_setsCache;
	GfIntrusiveList<DescSetEntry> m_perFrameOrderedSets;
	GfPool<DescSetEntry, 128> m_setsPool;
	GfVector<VkDescriptorPool> m_createdPools;

	VkDescriptorPool m_curPool;
	u32 m_avalSetsInPool;
};


class GfDescriptorSetFactoryVK
{
public:
	GfDescriptorSetFactoryVK(VkDevice device);

	void shutdown();

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