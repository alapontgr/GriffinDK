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

GfVariantDataVK GfShaderPipeline_Platform::getOrCreateGraphicsPipeline(const GfRenderContext& ctx, 
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
	GfVariantDataVK res;
	res.m_layout = ctx.Plat().getDescSetFactory()->getOrCreatePipelineLayout(deserializer, variantData);
	res.m_pipeline = createPipeline(ctx, variantData, config, vertexFormat, renderPass->Plat().getRenderPass(), res.m_layout);
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
	StackMemBlock vertexAttribsMem(static_cast<u32>(sizeof(VkVertexInputAttributeDescription)) * vertexFormat->getAttribCount());
	StackMemBlock vertexBufferBindingsMem(static_cast<u32>(sizeof(VkVertexInputBindingDescription)) * vertexFormat->getVertexBufferBindCount());
	VkVertexInputAttributeDescription* vertexAttributeDescriptions = reinterpret_cast<VkVertexInputAttributeDescription*>(vertexAttribsMem.get());
	VkVertexInputBindingDescription* vertexBindingDescriptions = reinterpret_cast<VkVertexInputBindingDescription*>(vertexBufferBindingsMem.get());
	VkPipelineVertexInputStateCreateInfo vertexCi{};
	vertexCi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexCi.pNext = nullptr;
	vertexCi.flags = 0;
	vertexCi.vertexAttributeDescriptionCount = vertexFormat->getAttribCount();
	vertexCi.vertexBindingDescriptionCount = vertexFormat->getVertexBufferBindCount();
	vertexCi.pVertexAttributeDescriptions = vertexAttributeDescriptions;
	vertexCi.pVertexBindingDescriptions = vertexBindingDescriptions;
	convertInputVertex(vertexFormat, vertexBindingDescriptions, vertexAttributeDescriptions);
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

void GfDescriptorSetFactoryVK::shutdown()
{
	GF_ASSERT_ALWAYS("TODO: Implement me!");
}

void GfDescriptorSetFactoryVK::tick()
{
	GF_ASSERT_ALWAYS("TODO: Implement me!");
}

VkPipelineLayout GfDescriptorSetFactoryVK::getOrCreatePipelineLayout(const GfShaderDeserializer& deserializer, const GfShaderVariantData* variantData)
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

	StackMemBlock bindingsArray(static_cast<u32>(sizeof(VkDescriptorSetLayout)) * usedSetCount);
	VkDescriptorSetLayout* setLayouts = reinterpret_cast<VkDescriptorSetLayout*>(bindingsArray.get());
	u32 populated = 0;
	while (populated < usedSetCount) 
	{
		u32 bindingCount = 0;
		u64 layoutHash = 0;
		const GfDescriptorBindingSlot* bindings = deserializer.getDescriptorBindings(variantData, populated, bindingCount, layoutHash);
		GfWeakArray<GfDescriptorBindingSlot> slotsArray(bindings, bindingCount);
		setLayouts[populated] = (variantData->m_setsLayoutHash[populated] != 0) ?
			getOrCreateDescriptorSetLayout(layoutHash, slotsArray) : m_emptyDescSetLayout;
		populated++;
	}
	ci.setLayoutCount = usedSetCount;
	ci.pSetLayouts = setLayouts;

	VkPipelineLayout pipelineLayout = nullptr;
	VkResult result = vkCreatePipelineLayout(m_device, &ci, nullptr, &pipelineLayout);
	GF_ASSERT(result == VK_SUCCESS, "Failed to create Pipeline layout");

	m_pipelineLayoutCache[pipelineLayoutHash] = pipelineLayout;

	return pipelineLayout;
}

VkDescriptorSetLayout GfDescriptorSetFactoryVK::getOrCreateDescriptorSetLayout(u64 hash, const GfWeakArray<GfDescriptorBindingSlot>& setBindings)
{
	GfLock<GfMutex> lock(m_descSetLayoutCacheMutex);
	auto entry = m_descSetLayoutCache.find(hash);
	if (entry != m_descSetLayoutCache.end()) 
	{
		return entry->second;
	}

	VkDescriptorSetLayout layout = createDescriptorSetLayout(setBindings);
	m_descSetLayoutCache[hash] = layout;
	return layout;
}

VkDescriptorSetLayout GfDescriptorSetFactoryVK::createDescriptorSetLayout(const GfWeakArray<GfDescriptorBindingSlot>& setBindings)
{	
	VkDescriptorSetLayoutCreateInfo ci{};
	ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	ci.pNext = nullptr;
	ci.flags = 0;
	ci.bindingCount = 0;
	ci.pBindings = nullptr;

	StackMemBlock bindingsArray(static_cast<u32>(sizeof(VkDescriptorSetLayoutBinding)) * setBindings.size());
	VkDescriptorSetLayoutBinding* bindings = reinterpret_cast<VkDescriptorSetLayoutBinding*>(bindingsArray.get());
	for (u32 i = 0; i < setBindings.size(); ++i) 
	{
		const GfDescriptorBindingSlot& slot = setBindings[i];
		bindings[i].binding = slot.m_bindingSlot;
		bindings[i].stageFlags = ConvertShaderStageFlags(slot.m_stageFlags);
		bindings[i].descriptorCount = slot.m_arraySize;
		bindings[i].descriptorType = ConvertDescriptorType(slot.m_descriptorType);
		bindings[i].pImmutableSamplers = nullptr;
	}

	ci.bindingCount = setBindings.size();
	ci.pBindings = bindings;

	VkDescriptorSetLayout layout;
	VkResult result = vkCreateDescriptorSetLayout(m_device, &ci, nullptr, &layout);
	GF_ASSERT(result == VK_SUCCESS, "Failed to create Desc Set Layout");
	return layout;
}
