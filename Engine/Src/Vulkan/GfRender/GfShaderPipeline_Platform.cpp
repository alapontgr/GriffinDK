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
#include "Common/GfRender/GfRenderContext.h"
#include "Common/GfRender/GfRenderPass.h"
#include "Common/GfRender/GfCmdBuffer.h"

////////////////////////////////////////////////////////////////////////////////

static void convertRasterState(const GfRasterState& kRasterState, VkPipelineRasterizationStateCreateInfo& kOut) 
{
	kOut.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	kOut.pNext = nullptr;
	kOut.flags = 0;
	kOut.depthBiasConstantFactor = kRasterState.m_depthBiasConstFactor;
	kOut.depthBiasClamp = kRasterState.m_depthBiasClamp;
	kOut.depthBiasSlopeFactor = kRasterState.m_depthBiasSlopeFactor;
	kOut.lineWidth = kRasterState.m_lineWidth;
	kOut.polygonMode = ConvertPolygonMode(kRasterState.m_polygonMode);
	kOut.cullMode = ConvertCullMode(kRasterState.m_cullMode);
	kOut.frontFace = ConvertFrontFace(kRasterState.m_frontFace);
	kOut.depthClampEnable = kRasterState.m_depthClampEnabled ? VK_TRUE : VK_FALSE;
	kOut.rasterizerDiscardEnable = kRasterState.m_rasterizerDiscardEnabled ? VK_TRUE : VK_FALSE;
	kOut.depthBiasEnable = kRasterState.m_depthBiasEnabled ? VK_TRUE : VK_FALSE;
}

static void convertMSState(const GfMultiSamplingState& msState, VkPipelineMultisampleStateCreateInfo& out) 
{
	out.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	out.pNext = nullptr;
	out.flags = 0;
	out.rasterizationSamples = ConvertSampleCount(msState.m_uiSampleCount);
	out.sampleShadingEnable = msState.m_bEnabled ? VK_TRUE : VK_FALSE;
	out.minSampleShading = 1.0f;
	out.pSampleMask = nullptr;
	out.alphaToCoverageEnable = VK_FALSE;
	out.alphaToOneEnable = VK_FALSE;
}

static void convertBlendState(const GfBlendState& blendState,
	VkPipelineColorBlendAttachmentState& blendAttachment, VkPipelineColorBlendStateCreateInfo& out)
{
	blendAttachment.blendEnable = blendState.m_bEnabled ? VK_TRUE : VK_FALSE;
	blendAttachment.srcColorBlendFactor = ConvertBlendFactor(blendState.m_srcColorBlendFactor);
	blendAttachment.dstColorBlendFactor = ConvertBlendFactor(blendState.m_dstColorBlendFactor);
	blendAttachment.colorBlendOp = ConvertBlendOp(blendState.m_colorBlendOp);
	blendAttachment.srcAlphaBlendFactor = ConvertBlendFactor(blendState.m_srcAlphaBlendFactor);
	blendAttachment.dstAlphaBlendFactor = ConvertBlendFactor(blendState.m_dstAlphaBlendFactor);
	blendAttachment.alphaBlendOp = ConvertBlendOp(blendState.m_alphaBlendOp);;
	blendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;

	out.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	out.pNext = nullptr;
	out.flags = 0;
	out.logicOpEnable = blendState.m_bLogicOpEnabled ? VK_TRUE : VK_FALSE;
	out.logicOp = ConvertBlendLogicOp(blendState.m_blendLogicOp);
	out.attachmentCount = 1;
	out.pAttachments = &blendAttachment;
	out.blendConstants[0] = blendState.m_blendConstants.x;
	out.blendConstants[1] = blendState.m_blendConstants.y;
	out.blendConstants[2] = blendState.m_blendConstants.z;
	out.blendConstants[3] = blendState.m_blendConstants.w;
}

static void convertInputAssembler(const PrimitiveTopology::Type topology, VkPipelineInputAssemblyStateCreateInfo& out) 
{
	out.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	out.pNext = nullptr;
	out.flags = 0;
	out.topology = ConvertTopology(topology);
	out.primitiveRestartEnable = VK_FALSE; // TODO
}

static void convertDepthStencilState(const GfDepthState& depthState, VkPipelineDepthStencilStateCreateInfo& depthStencilCi) 
{
	depthStencilCi.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilCi.pNext = nullptr;
	depthStencilCi.flags = 0;
	depthStencilCi.depthTestEnable = depthState.m_depthTestEnabled;
	depthStencilCi.depthWriteEnable = depthState.m_depthWriteEnabled;
	depthStencilCi.stencilTestEnable = depthState.m_stencilEnabled;
	depthStencilCi.depthCompareOp = ConvertCompareOp(depthState.m_depthCompareOp);
	depthStencilCi.depthBoundsTestEnable = false;
	depthStencilCi.minDepthBounds = 0.0f;
	depthStencilCi.maxDepthBounds = 1.0f;
	
	depthStencilCi.front.failOp = ConvertStencilOp(depthState.m_stencilFront.m_failOp);
	depthStencilCi.front.passOp = ConvertStencilOp(depthState.m_stencilFront.m_passOp);
	depthStencilCi.front.depthFailOp = ConvertStencilOp(depthState.m_stencilFront.m_depthFailOp);
	depthStencilCi.front.compareOp = ConvertCompareOp(depthState.m_stencilFront.m_compareOp);
	depthStencilCi.front.compareMask = 0;
	depthStencilCi.front.writeMask = 0;
	depthStencilCi.front.reference = 0;
	
	depthStencilCi.back.failOp = ConvertStencilOp(depthState.m_stencilBack.m_failOp);
	depthStencilCi.back.passOp = ConvertStencilOp(depthState.m_stencilBack.m_passOp);
	depthStencilCi.back.depthFailOp = ConvertStencilOp(depthState.m_stencilBack.m_depthFailOp);
	depthStencilCi.back.compareOp = ConvertCompareOp(depthState.m_stencilBack.m_compareOp);
	depthStencilCi.back.compareMask = 0;
	depthStencilCi.back.writeMask = 0;
	depthStencilCi.back.reference = 0;
}

static void convertInputVertex(const class GfVertexDeclaration* vertexFormat,
	VkVertexInputBindingDescription* outVertexBindings,
	VkVertexInputAttributeDescription* outVertexAttributes) 
{
	const auto& attribs = vertexFormat->getAttributes();
	for (u32 i = 0; i < vertexFormat->getAttribCount(); ++i) 
	{
		outVertexAttributes[i].binding = attribs[i].m_vertexBufferIdx;
		outVertexAttributes[i].location = attribs[i].m_location;
		outVertexAttributes[i].offset = attribs[i].m_uiOffset;
		outVertexAttributes[i].format = ConvertAttributeFormat(attribs[i].m_eType);
	}

	static VkVertexInputRate s_vertexRateTable[] = {VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX, VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE};
	const auto& vertexBuffers = vertexFormat->getVertexBuffers();
	for (u32 i = 0; i < vertexFormat->getVertexBufferBindCount(); ++i) 
	{
		outVertexBindings[i].binding = vertexBuffers[i].m_bufferIdx;
		outVertexBindings[i].stride = static_cast<u32>(vertexBuffers[i].m_stride);
		outVertexBindings[i].inputRate = s_vertexRateTable[vertexBuffers[i].m_rate];
	}
}

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfShaderPipeline) 
{
}

const GfVariantDataVK* GfShaderPipeline_Platform::getOrCreateGraphicsPipeline(const GfRenderContext& ctx, 
		const u64 hash,	
		const GfShaderVariantData* variantData,
		const GfShaderPipeConfig* config,
		const GfVertexDeclaration* vertexFormat,
		const GfRenderPass* renderPass)
{
	GfLock<GfMutex> lock(m_pipelineCacheMutex);
	auto entry = m_pipelineCache.find(hash);
	if (entry != m_pipelineCache.end()) 
	{
		return entry->second;
	}

	// Create it
	const GfShaderDeserializer& deserializer = m_kBase.getDeserializer();
	GfVariantDataVK* res = m_pipelinePool.pop();
	res->m_layoutData = ctx.Plat().getDescSetFactory()->getOrCreatePipelineLayout(deserializer, variantData);
	res->m_pipeline = createPipeline(ctx, variantData, config, vertexFormat, renderPass->Plat().getRenderPass(), res->m_layoutData->m_layout);
	m_pipelineCache[hash] = res;
	return res;
}

VkPipeline GfShaderPipeline_Platform::createPipeline(const GfRenderContext& ctx, 
	const GfShaderVariantData* variantData,
	const GfShaderPipeConfig* config, 
	const GfVertexDeclaration* vertexFormat, 
	VkRenderPass renderPass, 
	VkPipelineLayout layout)
{	
	VkGraphicsPipelineCreateInfo ci{};
	ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	ci.pNext = nullptr;
	ci.flags = 0;

	u32 usedStages = m_kBase.getUsedStages();
	u32 stageCount(0);
	for (u32 i = 0; i < ShaderStage::Count; ++i) 
	{
		if (((1 << i) & usedStages) != 0) 
		{
			stageCount++;
		}
	}

	// Fill shader modules
	StackMemBlock stagesCIMem(static_cast<u32>(sizeof(VkPipelineShaderStageCreateInfo)) * stageCount);
	VkPipelineShaderStageCreateInfo* stages = reinterpret_cast<VkPipelineShaderStageCreateInfo*>(stagesCIMem.get());
	populateShaderStages(ctx, variantData, usedStages, stages);
	ci.stageCount = stageCount;
	ci.pStages = stages;

	// Fill vertex definition
	u32 atribCount(vertexFormat ? vertexFormat->getAttribCount() : 0);
	u32 vertexBufferBindingsCount(vertexFormat ? vertexFormat->getVertexBufferBindCount() : 0);

	StackMemBlock vertexAttribsMem(static_cast<u32>(sizeof(VkVertexInputAttributeDescription)) * atribCount);
	StackMemBlock vertexBufferBindingsMem(static_cast<u32>(sizeof(VkVertexInputBindingDescription)) * vertexBufferBindingsCount);
	VkVertexInputAttributeDescription* vertexAttributeDescriptions = reinterpret_cast<VkVertexInputAttributeDescription*>(vertexAttribsMem.get());
	VkVertexInputBindingDescription* vertexBindingDescriptions = reinterpret_cast<VkVertexInputBindingDescription*>(vertexBufferBindingsMem.get());
	VkPipelineVertexInputStateCreateInfo vertexCi{};
	vertexCi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexCi.pNext = nullptr;
	vertexCi.flags = 0;
	vertexCi.vertexAttributeDescriptionCount = atribCount;
	vertexCi.vertexBindingDescriptionCount = vertexBufferBindingsCount;
	vertexCi.pVertexAttributeDescriptions = vertexAttributeDescriptions;
	vertexCi.pVertexBindingDescriptions = vertexBindingDescriptions;
	if (vertexFormat) 
	{
		convertInputVertex(vertexFormat, vertexBindingDescriptions, vertexAttributeDescriptions);
	}
	ci.pVertexInputState = &vertexCi;

	// Input assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssembler{};
	convertInputAssembler(config->m_topology, inputAssembler);
	ci.pInputAssemblyState = &inputAssembler;

	// Tessellation state
	ci.pTessellationState = nullptr;

	// Viewport state
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.pNext = nullptr;
	viewportState.flags = 0;
	viewportState.viewportCount = 1;
	viewportState.pViewports = nullptr; //&viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = nullptr; //&scissors;
	ci.pViewportState = &viewportState;

	// Raster state
	VkPipelineRasterizationStateCreateInfo rasterState{};
	convertRasterState(config->m_rasterState, rasterState);
	ci.pRasterizationState = &rasterState;

	// MultiSample state
	VkPipelineMultisampleStateCreateInfo multiSampleState{};
	convertMSState(config->m_msaState, multiSampleState);
	ci.pMultisampleState = &multiSampleState;

	// Depth stencil state
	VkPipelineDepthStencilStateCreateInfo  depthStencilCi{};
	convertDepthStencilState(config->m_depthState, depthStencilCi);
	ci.pDepthStencilState = &depthStencilCi;

	// Blend State
	VkPipelineColorBlendStateCreateInfo blendStateCi{};
	VkPipelineColorBlendAttachmentState blendAttachmentCi{};
	convertBlendState(config->m_blendState, blendAttachmentCi, blendStateCi);
	ci.pColorBlendState = &blendStateCi;

	// Dynamic State
	VkDynamicState dynamicStates[] = 
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_DEPTH_BIAS,
		VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
		VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
		VK_DYNAMIC_STATE_STENCIL_REFERENCE,
	};
	VkPipelineDynamicStateCreateInfo dynStateInfo{};
	dynStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynStateInfo.pNext = nullptr;
	dynStateInfo.flags = 0;
	dynStateInfo.dynamicStateCount = (sizeof(dynamicStates) / sizeof(dynamicStates[0]));
	dynStateInfo.pDynamicStates = dynamicStates;
	ci.pDynamicState = &dynStateInfo;

	// Rest
	ci.layout = layout;
	ci.renderPass = renderPass;
	ci.subpass = 0;
	ci.basePipelineHandle = VK_NULL_HANDLE;
	ci.basePipelineIndex = 0;

	VkPipeline pipeline = VK_NULL_HANDLE;
	VkResult result = vkCreateGraphicsPipelines(ctx.Plat().m_pDevice, VK_NULL_HANDLE, 1, &ci, nullptr, &pipeline);
	GF_ASSERT(result == VK_SUCCESS, "Failed to create graphics pipeline");
	return pipeline;
}

void GfShaderPipeline_Platform::populateShaderStages(const GfRenderContext& ctx, 
	const GfShaderVariantData* variantData,
	const u32 usedStages,
	VkPipelineShaderStageCreateInfo* stagesToFill)
{
	u32 idx = 0;
	for (u32 i = 0; i < ShaderStage::Count; ++i) 
	{
		if ((usedStages & (1 << i)) != 0) 
		{
			stagesToFill[idx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stagesToFill[idx].pNext = nullptr;
			stagesToFill[idx].flags = 0;
			stagesToFill[idx].stage = ConvertShaderStage(i);
			stagesToFill[idx].pName = "main";
			stagesToFill[idx].pSpecializationInfo = nullptr;
			stagesToFill[idx].module = getOrCreateModule(ctx, variantData, static_cast<ShaderStage::Type>(i));
			idx++;
		}
	}
}

VkShaderModule GfShaderPipeline_Platform::getOrCreateModule(const GfRenderContext& ctx,
	const GfShaderVariantData* variantData, ShaderStage::Type stage) 
{
	s32 bytecodeIdx = variantData->m_stagesBytecodeIdxs[stage];
	GF_ASSERT(bytecodeIdx >= 0, "Something went wrong, the stage is active but missing shader bytecode");
	auto it = m_modulesCache.find(bytecodeIdx);
	if (it != m_modulesCache.end()) 
	{
		return it->second;
	}

	const GfShaderDeserializer& deserializer = m_kBase.getDeserializer();
	VkShaderModule module = VK_NULL_HANDLE;
	VkShaderModuleCreateInfo shaderCi{};
	shaderCi.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCi.pNext = nullptr;
	shaderCi.flags = 0;
	shaderCi.pCode = deserializer.getStageBytecodeForVariant(variantData, stage, shaderCi.codeSize);
	vkCreateShaderModule(ctx.Plat().m_pDevice, &shaderCi, nullptr, &module);
	m_modulesCache[bytecodeIdx] = module;
	return module;
}

GfDescriptorSetFactoryVK::GfDescriptorSetFactoryVK(VkDevice device) 
	: m_device(device)
{
	// Use empty layout to fill gaps
	GfWeakArray<GfDescriptorBindingSlot> emptyArray(nullptr, 0);
	m_emptyDescSetLayout.init(createDescriptorSetLayout(emptyArray), emptyArray);
}

void GfDescriptorSetFactoryVK::shutdown()
{
	GF_ASSERT_ALWAYS("TODO: Implement me!");
}

const GfLayoutDataVK* GfDescriptorSetFactoryVK::getOrCreatePipelineLayout(const GfShaderDeserializer& deserializer, const GfShaderVariantData* variantData)
{
	GfLock<GfMutex> lock(m_pipelineLayoutCacheMutex);

	u64 pipelineLayoutHash = GfHash::compute(variantData->m_setsLayoutHash.data(), variantData->m_setsLayoutHash.size() * sizeof(u64));

	auto entry = m_pipelineLayoutCache.find(pipelineLayoutHash);
	if (entry != m_pipelineLayoutCache.end()) 
	{
		return entry->second;
	}

	u32 usedSetCount = 0;
	// Find the highest index of a used Desc Set
	for (u32 i = s_MAX_DESCRIPTOR_SETS-1; i>0; --i) 
	{
		if (variantData->m_setsLayoutHash[i] != 0) 
		{
			usedSetCount = i+1;
			break;
		}
	}

	VkPipelineLayoutCreateInfo ci{};
	ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	ci.pNext = nullptr;
	ci.pushConstantRangeCount = 0;
	ci.pPushConstantRanges = nullptr;
	ci.setLayoutCount = 0;
	ci.pSetLayouts = nullptr;
	ci.flags = 0;

	GfLayoutDataVK* layout = m_layoutDataPool.pop();
	layout->m_setLayoutCaches.fill(VK_NULL_HANDLE);

	StackMemBlock bindingsArray(static_cast<u32>(sizeof(VkDescriptorSetLayout)) * usedSetCount);
	VkDescriptorSetLayout* setLayouts = reinterpret_cast<VkDescriptorSetLayout*>(bindingsArray.get());
	for (u32 i = 0; i<usedSetCount; ++i) 
	{
		u32 bindingCount = 0;
		u64 layoutHash = 0;
		GfWeakArray<GfDescriptorBindingSlot> bindings(deserializer.getDescriptorBindings(variantData, i, layoutHash));
		layout->m_setLayoutCaches[i] = (variantData->m_setsLayoutHash[i] != 0) ?
			getOrCreateDescriptorSetLayoutCache(layoutHash, bindings) : &m_emptyDescSetLayout;
		setLayouts[i] = layout->m_setLayoutCaches[i]->getLayout();
	}
	ci.setLayoutCount = usedSetCount;
	ci.pSetLayouts = setLayouts;

	VkResult result = vkCreatePipelineLayout(m_device, &ci, nullptr, &layout->m_layout);
	GF_ASSERT(result == VK_SUCCESS, "Failed to create Pipeline layout");

	m_pipelineLayoutCache[pipelineLayoutHash] = layout;

	return layout;
}

GfDescriptorSetCache* GfDescriptorSetFactoryVK::getOrCreateDescriptorSetLayoutCache(u64 hash, const GfWeakArray<GfDescriptorBindingSlot>& setBindings)
{
	GfLock<GfMutex> lock(m_descSetLayoutCacheMutex);
	auto entry = m_descSetCacheCache.find(hash);
	if (entry != m_descSetCacheCache.end()) 
	{
		return entry->second;
	}

	GfDescriptorSetCache* res = m_descSetCachePool.pop();
	res->init(createDescriptorSetLayout(setBindings), setBindings);
	m_descSetCacheCache[hash] = res;
	return res;
}

VkDescriptorSetLayout GfDescriptorSetFactoryVK::createDescriptorSetLayout(const GfWeakArray<GfDescriptorBindingSlot>& setBindings)
{	
	VkDescriptorSetLayoutCreateInfo ci{};
	ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	ci.pNext = nullptr;
	ci.flags = 0;
	ci.bindingCount = 0;
	ci.pBindings = nullptr;

	u32 bindingCount(0);
	for (u32 i = 0; i < setBindings.size(); ++i) 
	{
		if (setBindings[i].m_descriptorType != GfParameterSlotType::Invalid) 
		{
			bindingCount++;
		}
	}

	StackMemBlock bindingsArray(static_cast<u32>(sizeof(VkDescriptorSetLayoutBinding)) * bindingCount);
	VkDescriptorSetLayoutBinding* bindings = reinterpret_cast<VkDescriptorSetLayoutBinding*>(bindingsArray.get());
	u32 currPopulated(0);
	for (u32 i = 0; i < setBindings.size(); ++i) 
	{
		if (setBindings[i].m_descriptorType != GfParameterSlotType::Invalid) 
		{
			const GfDescriptorBindingSlot& slot = setBindings[i];
			bindings[currPopulated].binding = i;
			bindings[currPopulated].stageFlags = ConvertShaderStageFlags(slot.m_stageFlags);
			bindings[currPopulated].descriptorCount = slot.m_arraySize;
			bindings[currPopulated].descriptorType = ConvertDescriptorType(slot.m_descriptorType);
			bindings[currPopulated].pImmutableSamplers = nullptr;
			currPopulated++;
		}
	}

	ci.bindingCount = bindingCount;
	ci.pBindings = bindings;

	VkDescriptorSetLayout layout;
	VkResult result = vkCreateDescriptorSetLayout(m_device, &ci, nullptr, &layout);
	GF_ASSERT(result == VK_SUCCESS, "Failed to create Desc Set Layout");
	return layout;
}

GfDescriptorSetCache::GfDescriptorSetCache()
	: m_layout(VK_NULL_HANDLE)
	, m_curPool(VK_NULL_HANDLE)
	, m_avalSetsInPool(0)
{
}

void GfDescriptorSetCache::init(VkDescriptorSetLayout layout, const GfWeakArray<GfDescriptorBindingSlot>& setBindings)
{
	m_layout = layout;
	GF_ASSERT(setBindings.size() <= s_MAX_BINDINGS_PER_SET, "Invalid number of bindings");	
	m_perResourceTypeCount.fill(0);
	for (u32 i = 0; i < setBindings.size(); ++i) 
	{
		if (setBindings[i].m_descriptorType != GfParameterSlotType::Invalid) 
		{
			m_perResourceTypeCount[setBindings[i].m_descriptorType] += setBindings[i].m_arraySize;
		}
	}
}

VkDescriptorSet GfDescriptorSetCache::getOrCreateDescSet(
	const GfRenderContext& ctx,
	GfLinearAllocator& alloc,
	const GfWeakArray<GfDescriptorBindingSlot>& setBindings,
	const GfWeakArray<GfResourceBindingExt>& resourceBindings) 
{
	u64 bindingsHash = computeBindingsHash(alloc, setBindings, resourceBindings);

	auto entry = m_setsCache.find(bindingsHash);
	if (entry != m_setsCache.end()) 
	{
		entry->second->m_lastUsedFrame = ctx.getCurFrame();
		m_perFrameOrderedSets.erase(entry->second);
		m_perFrameOrderedSets.pushBack(entry->second);
		return entry->second->m_set;
	}

	// Failed to find a set, try to reuse one
	u32 curFrame = ctx.getCurFrame();
	DescSetEntry* result = m_perFrameOrderedSets.front(); // Oldest Desc Set
	if (result && (result->m_lastUsedFrame - curFrame) > ms_descSetBufferingFrames) 
	{
		// Reuse
		m_setsCache.erase(result->m_hash);
		m_perFrameOrderedSets.erase(result);
		m_perFrameOrderedSets.pushBack(result);
	}
	else 
	{
		// Create new one
		result = m_setsPool.pop();
		GF_ASSERT(result, "Failed to allocate");
		m_perFrameOrderedSets.pushBack(entry->second);
		result->m_set = createDescSet(ctx, setBindings);
	}

	result->m_hash = bindingsHash;
	result->m_lastUsedFrame = curFrame;
	m_setsCache[bindingsHash] = result;

	// Update set bindings
	updateDescSet(ctx, alloc, result->m_set, setBindings, resourceBindings);

	GF_ASSERT(result && result->m_set != VK_NULL_HANDLE, "Something went wrong!");
	return result->m_set;
}

u64 GfDescriptorSetCache::computeBindingsHash(
	GfLinearAllocator& alloc,
	const GfWeakArray<GfDescriptorBindingSlot>& setBindings,
	const GfWeakArray<GfResourceBindingExt>& resourceBindings) const 
{
	size_t reqSize(0);
	for (u32 i = 0; i < s_MAX_BINDINGS_PER_SET; ++i) 
	{
		if (setBindings[i].m_descriptorType != GfParameterSlotType::Invalid) 
		{
			u32 elementCount(glm::min(setBindings[i].m_arraySize, resourceBindings[i].m_arrayCount));
			reqSize += sizeof(u64) * elementCount;

			switch (setBindings[i].m_descriptorType) 
			{
			case GfParameterSlotType::UniformBuffer:
			case GfParameterSlotType::StorageBuffer:
				reqSize += sizeof(GfResourceBindingEntry::BufferBinding) * elementCount;
				break;
			case GfParameterSlotType::StorageImage:
				reqSize += sizeof(GfResourceBindingEntry::ImageBinding) * elementCount;
				break;
			case GfParameterSlotType::CombinedImageSampler:
				reqSize += sizeof(GfResourceBindingEntry::CombinedSamplerTexture) * elementCount;
				break;
			case GfParameterSlotType::SampledImage:
				reqSize += sizeof(GfResourceBindingEntry::SampledTextureBinding) * elementCount;
				break;
			case GfParameterSlotType::Sampler:
				reqSize += sizeof(GfResourceBindingEntry::SamplerBinding) * elementCount;
				break;
			default:
				GF_ERROR("Trying to use non supported resource");
				break;
			}
		}
	}

	static constexpr u32 s_StackMemLimit = 1024;
	bool allocFromStack = (reqSize < s_StackMemLimit);
	u32 allocateFromStack(allocFromStack ? reqSize : 0);
	StackMemBlock toHashStackMemBlock(allocateFromStack);
	GfLinearAllocator::Block linearMemBlock(alloc.allocBlock(allocFromStack ? 0 : reqSize));
	// If tiny enough use stack memory
	u8* toHash = reinterpret_cast<u8*>(allocFromStack ?	toHashStackMemBlock.get() : linearMemBlock.get());
	size_t pivot(0);
	
	#define ADD_RES_TO_HASH(TYPE, VAR) *reinterpret_cast<TYPE*>(toHash + pivot) = (entry->VAR); pivot += sizeof(TYPE);

	for (u32 i = 0; i < s_MAX_BINDINGS_PER_SET; ++i)
	{
		if (setBindings[i].m_descriptorType != GfParameterSlotType::Invalid) 
		{
			u32 elementCount(glm::min(setBindings[i].m_arraySize, resourceBindings[i].m_arrayCount));
			const GfResourceBindingEntry* entry(resourceBindings[i].m_front);
			while (entry) 
			{
				GF_ASSERT(entry->m_type == setBindings[i].m_descriptorType, "Invalid resource type");

				// Add the binding indx
				ADD_RES_TO_HASH(u64, m_bindIdx);
				// Add resources to hash
				switch (setBindings[i].m_descriptorType) 
				{
				case GfParameterSlotType::UniformBuffer:
				case GfParameterSlotType::StorageBuffer:
					ADD_RES_TO_HASH(GfResourceBindingEntry::BufferBinding, m_bufferBind);
					break;
				case GfParameterSlotType::StorageImage:
					ADD_RES_TO_HASH(GfResourceBindingEntry::ImageBinding, m_imageBind);
					break;
				case GfParameterSlotType::CombinedImageSampler:
					ADD_RES_TO_HASH(GfResourceBindingEntry::CombinedSamplerTexture, m_combinedSamplerTextureBind);
					break;
				case GfParameterSlotType::SampledImage:
					ADD_RES_TO_HASH(GfResourceBindingEntry::SampledTextureBinding, m_sampledTextureBind);
					break;
				case GfParameterSlotType::Sampler:
					ADD_RES_TO_HASH(GfResourceBindingEntry::SamplerBinding, m_samplerBind);
					break;
				default:
					GF_ERROR("Trying to use non supported resource");
					break;
				}

				// Next
				entry = entry->m_next;
			}
		}
	}
	#undef ADD_RES_TO_HASH

	GF_ASSERT(pivot == reqSize, "Something went wrong, buggy!");
	return GfHash::compute(toHash, reqSize);
}

VkDescriptorSet GfDescriptorSetCache::createDescSet(
	const GfRenderContext& ctx,
	const GfWeakArray<GfDescriptorBindingSlot>& setBindings) 
{
	VkDescriptorSetAllocateInfo setAllocInfo{};
	setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocInfo.pNext = nullptr;
	setAllocInfo.descriptorSetCount = 1;
	setAllocInfo.pSetLayouts = &m_layout;
	setAllocInfo.descriptorPool = getPoolToAllocateFrom(ctx);

	VkDescriptorSet set(VK_NULL_HANDLE);
	VkResult resVk = vkAllocateDescriptorSets(ctx.Plat().m_pDevice, &setAllocInfo, &set);
	GF_ASSERT(resVk == VK_SUCCESS, "Failed to allocate Descriptor set");
	return set;
}

static bool isBufferType(const GfParameterSlotType::Type type) 
{
	return (type == GfParameterSlotType::UniformBuffer) ||
		(type == GfParameterSlotType::StorageBuffer);
}

void GfDescriptorSetCache::updateDescSet(
	const GfRenderContext& ctx,
	GfLinearAllocator& alloc,
	const VkDescriptorSet set,
	const GfWeakArray<GfDescriptorBindingSlot>& setBindings,
	const GfWeakArray<GfResourceBindingExt>& resourceBindings) const 
{
	u32 reqImageWrites(0);
	u32 reqBufferWrites(0);
	u32 reqDescWrites(0);
	// Figure out how many descriptor writes per type to allocate
	for (u32 i=0; i<s_MAX_BINDINGS_PER_SET; ++i) 
	{
		if (setBindings[i].m_descriptorType != GfParameterSlotType::Invalid) 
		{		
			u32 elementCount(glm::min(setBindings[i].m_arraySize, resourceBindings[i].m_arrayCount));
			switch (setBindings[i].m_descriptorType) 
			{
			case GfParameterSlotType::Sampler:
			case GfParameterSlotType::CombinedImageSampler:
			case GfParameterSlotType::SampledImage:
			case GfParameterSlotType::StorageImage:
				reqImageWrites += elementCount;
				break;
			case GfParameterSlotType::UniformBuffer:
			case GfParameterSlotType::StorageBuffer:
				reqBufferWrites += elementCount;
				break;
			default:
				GF_ERROR("Trying to use non supported resource");
				break;
			}
		}
	}
	reqDescWrites = reqImageWrites + reqBufferWrites; // TODO: do batching of ranges of the array

	GfLinearAllocator::Block imageWritesBlock(alloc.allocBlock(sizeof(VkDescriptorImageInfo) * reqImageWrites));
	GfLinearAllocator::Block bufferWritesBlock(alloc.allocBlock(sizeof(VkDescriptorBufferInfo) * reqBufferWrites));
	GfLinearAllocator::Block descriptorWritesBlock(alloc.allocBlock(sizeof(VkWriteDescriptorSet) * reqDescWrites));

	VkDescriptorImageInfo* imageWrites = reinterpret_cast<VkDescriptorImageInfo*>(imageWritesBlock.get());
	u32 imageWritesPivot(0);
	VkDescriptorBufferInfo* bufferWrites = reinterpret_cast<VkDescriptorBufferInfo*>(bufferWritesBlock.get());
	u32 bufferWritesPivot(0);
	VkWriteDescriptorSet* descriptorWrites = reinterpret_cast<VkWriteDescriptorSet*>(descriptorWritesBlock.get());
	u32 descriptorWritesPivot(0);

	for (u32 i=0; i<s_MAX_BINDINGS_PER_SET; ++i) 
	{
		if (setBindings[i].m_descriptorType != GfParameterSlotType::Invalid) 
		{
			const GfResourceBindingEntry* entry(resourceBindings[i].m_front);
			while (entry && entry->m_bindIdx < setBindings[i].m_arraySize) 
			{
				VkWriteDescriptorSet* write(descriptorWrites+descriptorWritesPivot);
				descriptorWritesPivot++;

				write->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write->pNext = nullptr;
				write->dstSet = set;
				write->dstBinding = entry->m_bindIdx;
				write->dstArrayElement = 1;
				write->descriptorType = ConvertDescriptorType(setBindings[i].m_descriptorType);
				write->pTexelBufferView = nullptr;
				VkDescriptorImageInfo* imageW(nullptr);
				VkDescriptorBufferInfo* bufferW(nullptr);
				if (isBufferType(setBindings[i].m_descriptorType)) 
				{
					bufferW = (bufferWrites + bufferWritesPivot);
					write->pBufferInfo = (bufferWrites + bufferWritesPivot);
					bufferWritesPivot++; // TODO: Change this with batching
				}
				else 
				{
					imageW = (imageWrites + imageWritesPivot);
					write->pImageInfo = (imageWrites + imageWritesPivot);
					imageWritesPivot++; // TODO: Change this with batching
				}

				switch (setBindings[i].m_descriptorType) 
				{
				case GfParameterSlotType::Sampler:
					imageW[i].sampler = entry->m_samplerBind.m_sampler;
					break;
				case GfParameterSlotType::CombinedImageSampler:
					imageW[i].sampler = entry->m_combinedSamplerTextureBind.m_sampler;
					imageW[i].imageView = entry->m_combinedSamplerTextureBind.m_view;
					imageW[i].imageLayout = entry->m_combinedSamplerTextureBind.m_layout;
					break;
				case GfParameterSlotType::SampledImage:
					imageW[i].imageView = entry->m_sampledTextureBind.m_view;
					imageW[i].imageLayout = entry->m_sampledTextureBind.m_layout;
					break;
				case GfParameterSlotType::StorageImage:
					imageW[i].imageView = entry->m_imageBind.m_view;
					imageW[i].imageLayout = entry->m_imageBind.m_layout;
					break;
				case GfParameterSlotType::UniformBuffer:
				case GfParameterSlotType::StorageBuffer:
					bufferW[i].buffer = entry->m_bufferBind.m_buffer;
					bufferW[i].offset = entry->m_bufferBind.m_offset;
					bufferW[i].range = entry->m_bufferBind.m_size;
					break;
				default:
					GF_ERROR("Trying to use non supported resource");
					break;
				}
				// Next
				entry = entry->m_next;
			}			
		}
	}
	vkUpdateDescriptorSets(ctx.Plat().m_pDevice, reqDescWrites, descriptorWrites, 0, nullptr);
}

VkDescriptorPool GfDescriptorSetCache::getPoolToAllocateFrom(const GfRenderContext& ctx) 
{
	if (m_avalSetsInPool > 0) 
	{
		return m_curPool;
	}

	GfArray<VkDescriptorPoolSize, GfParameterSlotType::Count> descriptorSizes;
	u32 pivot(0);
	for (u32 i = 0; i < GfParameterSlotType::Count; ++i) 
	{
		if (m_perResourceTypeCount[i] > 0) 
		{
			descriptorSizes[pivot].type = ConvertDescriptorType(i);
			descriptorSizes[pivot].descriptorCount = m_perResourceTypeCount[i] * ms_descSetCountInPool;
			pivot++;
		}
	}

	VkDescriptorPoolCreateInfo  createPoolCI{};
	createPoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createPoolCI.pNext = nullptr;
	createPoolCI.flags = 0;
	createPoolCI.maxSets = ms_descSetCountInPool;
	createPoolCI.poolSizeCount = pivot;
	createPoolCI.pPoolSizes = descriptorSizes.data();
	VkResult createPoolRes = vkCreateDescriptorPool(ctx.Plat().m_pDevice, &createPoolCI, nullptr, &m_curPool);
	GF_ASSERT(createPoolRes == VK_SUCCESS, "Failed to create DescriptorPool");
	m_avalSetsInPool = ms_descSetCountInPool;
	m_createdPools.push_back(m_curPool);
	return m_curPool;
}