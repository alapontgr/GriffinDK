////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/26
//	File: 	GfMaterial_Platform.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfMaterial.h"
#include "GfRender/Common/GfMatParamLayout.h"
#include "GfRender/Common/GfRenderContext.h"

////////////////////////////////////////////////////////////////////////////////

static inline void ConvertRasterState(const GfRasterState& kRasterState, VkPipelineRasterizationStateCreateInfo& kOut) 
{
	kOut.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	kOut.pNext = nullptr;
	kOut.flags = 0;
	kOut.depthBiasConstantFactor = kRasterState.m_fDepthBiasConstFactor;
	kOut.depthBiasClamp = kRasterState.m_fDepthBiasClamp;
	kOut.depthBiasSlopeFactor = kRasterState.m_fDepthBiasSlopeFactor;
	kOut.lineWidth = kRasterState.m_fLineWidth;
	kOut.polygonMode = ConvertPolygonMode(kRasterState.m_ePolygonMode);
	kOut.cullMode = ConvertCullMode(kRasterState.m_eCullMode);
	kOut.frontFace = ConvertFrontFace(kRasterState.m_eFrontFace);
	kOut.depthClampEnable = kRasterState.m_bDepthClampEnabled ? VK_TRUE : VK_FALSE;
	kOut.rasterizerDiscardEnable = kRasterState.m_bRasterizerDiscardEnabled ? VK_TRUE : VK_FALSE;
	kOut.depthBiasEnable = kRasterState.m_bDepthBiasEnabled ? VK_TRUE : VK_FALSE;
}

////////////////////////////////////////////////////////////////////////////////

static inline void ConvertMSState(const GfMultiSamplingState& kMSState, VkPipelineMultisampleStateCreateInfo& kOut) 
{
	VkPipelineMultisampleStateCreateInfo multiSamplingInfo{};
	kOut.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	kOut.pNext = nullptr;
	kOut.flags = 0;
	kOut.rasterizationSamples = ConvertSampleCount(kMSState.m_uiSampleCount);
	kOut.sampleShadingEnable = kMSState.m_bEnabled ? VK_TRUE : VK_FALSE;
	kOut.minSampleShading = 1.0f;
	kOut.pSampleMask = nullptr;
	kOut.alphaToCoverageEnable = VK_FALSE;
	kOut.alphaToOneEnable = VK_FALSE;
}

////////////////////////////////////////////////////////////////////////////////

static inline void ConvertBlendState(const GfBlendState& kBlendState, VkPipelineColorBlendAttachmentState& kOut)
{

}

////////////////////////////////////////////////////////////////////////////////

static inline void ConvertTopology(const EPrimitiveTopology::Type eTopology, VkPipelineInputAssemblyStateCreateInfo& kOut) 
{
	kOut.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	kOut.pNext = nullptr;
	kOut.flags = 0;
	kOut.topology = ConvertTopology(eTopology);
	kOut.primitiveRestartEnable = VK_FALSE; // TODO
}

////////////////////////////////////////////////////////////////////////////////

static inline ConvertInputVertex(const GfVertexDeclaration& kVertex, u32 uiBinding,
	VkVertexInputBindingDescription& kOutVertexDesc,
	VkVertexInputAttributeDescription* pOutVertexAttributes)
{
	// IMPORTANT: pOutVertexAttributes must be a valid array of "kVertex.GetAttribCount()" elements 
	kOutVertexDesc.binding = uiBinding;
	kOutVertexDesc.stride = kVertex.GetStride();
	kOutVertexDesc.inputRate = ConvertInputRate(kVertex.GetRate());

	u32 uiAttribCount(kVertex.GetAttribCount());
	for (u32 i = 0; i < uiAttribCount; ++i) 
	{
		GfVertexAttrib kAttrib(kVertex.GetAttrib(i));
		pOutVertexAttributes[i].binding = uiBinding;
		pOutVertexAttributes[i].format = ConvertAttributeFormat(kAttrib.m_eType);
		pOutVertexAttributes[i].location = i;
		pOutVertexAttributes[i].offset = static_cast<u32>(kAttrib.m_uiOffset);
	}
}

////////////////////////////////////////////////////////////////////////////////

GfMaterialTemplate_Platform::GfMaterialTemplate_Platform(GfMaterialTemplate& kBase)
	: m_kBase(kBase)
	, m_pLayout(nullptr)
	, m_pPipeline(nullptr)
{

}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialTemplate_Platform::DestroyRHI(const GfRenderContext& kCtx)
{
	if (m_pPipeline)
	{
		vkDestroyPipeline(kCtx.m_pDevice, m_pPipeline, nullptr);
		m_pPipeline = nullptr;
	}
	if (m_pLayout) 
	{
		vkDestroyPipelineLayout(kCtx.m_pDevice, m_pLayout, nullptr);
		m_pLayout = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialTemplate_Platform::CreateRHI(const GfRenderContext& kCtx)
{
	DestroyRHI(kCtx);
	CreateLayout(kCtx);
	CreatePipeline(kCtx);
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialTemplate_Platform::CreateLayout(const GfRenderContext& kCtx)
{
	u32 uiLayoutCount(m_kBase.GetBoundLayoutCount());
	GfFrameMTStackAlloc::GfMemScope kMemScope(GfFrameMTStackAlloc::Get());
	VkDescriptorSetLayout* pLayouts(GfFrameMTStackAlloc::Get()->Alloc<VkDescriptorSetLayout>(uiLayoutCount));

	VkDescriptorSetLayout* pCursor(pLayouts);
	for (u32 i = 0; i < EMaterialParamRate::MaxBoundSets; ++i) 
	{
		GfMatParamLayout* pLayout(m_kBase.m_pLayouts[i]);
		if (pLayout) 
		{
			*pCursor = pLayout->GetLayout();
			pCursor++;
		}
	}

	VkPipelineLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.pNext = nullptr;
	layoutInfo.flags = 0;
	layoutInfo.setLayoutCount = uiLayoutCount;
	layoutInfo.pSetLayouts = pLayouts;
	layoutInfo.pushConstantRangeCount = 0;
	layoutInfo.pPushConstantRanges = nullptr;
	VkResult siResult = vkCreatePipelineLayout(kCtx.m_pDevice, &layoutInfo, nullptr, &m_pLayout);
	GF_ASSERT(siResult, "Failed to create Pipeline Layout");
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialTemplate_Platform::CreatePipeline(const GfRenderContext& kCtx)
{
	// Define Input Binding Desc

	VkVertexInputBindingDescription kVertexBindingDesc{};
	kVertexBindingDesc.binding = 0; // TODO: Add support for multiple vertex buffers
	
#ifdef DEAD

	// Load shader modules
	VkShaderModule vsMmodule = create_shader_module(RenderInterface::s_device, m_desc.m_vsPath.c_str());
	VkShaderModule psMmodule = create_shader_module(RenderInterface::s_device, m_desc.m_psPath.c_str());

	// Define stages
	static constexpr u32 kStageCount = 2;
	static VkPipelineShaderStageCreateInfo kStageInfo[kStageCount];
	define_stage_info(VK_SHADER_STAGE_VERTEX_BIT, vsMmodule, "main", &kStageInfo[0]);
	define_stage_info(VK_SHADER_STAGE_FRAGMENT_BIT, psMmodule, "main", &kStageInfo[1]);

	// Parse input assembler format
	std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions(1);
	vertexBindingDescriptions[0].binding = 0;
	vertexBindingDescriptions[0].stride = sizeof(VertexDesc);
	vertexBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions(1); // One for each parameter in the input layout
	vertexAttributeDescriptions[0].location = 0;
	vertexAttributeDescriptions[0].binding = vertexBindingDescriptions[0].binding;
	vertexAttributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	vertexAttributeDescriptions[0].offset = 0;
	//vertexAttributeDescriptions[1].location = 1;
	//vertexAttributeDescriptions[1].binding = vertexBindingDescriptions[0].binding;
	//vertexAttributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	//vertexAttributeDescriptions[1].offset = offsetof(struct VertexDesc, colour);

	VkPipelineVertexInputStateCreateInfo vertexShaderInfo{};
	vertexShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexShaderInfo.pNext = nullptr;
	vertexShaderInfo.flags = 0;
	vertexShaderInfo.vertexBindingDescriptionCount = static_cast<u32>(vertexBindingDescriptions.size());
	vertexShaderInfo.pVertexBindingDescriptions = &vertexBindingDescriptions[0];
	vertexShaderInfo.vertexAttributeDescriptionCount = static_cast<u32>(vertexAttributeDescriptions.size());
	vertexShaderInfo.pVertexAttributeDescriptions = &vertexAttributeDescriptions[0];

	// Define topology to feed the shaders
	VkPipelineInputAssemblyStateCreateInfo assemblerInfo;
	assemblerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assemblerInfo.pNext = nullptr;
	assemblerInfo.flags = 0;
	assemblerInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assemblerInfo.primitiveRestartEnable = VK_FALSE;

	// Define default viewport and scissors
	VkViewport viewport;
	viewport.x = 0.0f; // Left
	viewport.y = 0.0f; // Top
	viewport.width = 512.0f;
	viewport.height = 512.0f;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissors;
	scissors.offset.x = 0;
	scissors.offset.y = 0;
	scissors.extent.width = 512;
	scissors.extent.height = 512;

	VkPipelineViewportStateCreateInfo viewportStateInfo;
	viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfo.pNext = nullptr;
	viewportStateInfo.flags = 0;
	viewportStateInfo.viewportCount = 1;
	viewportStateInfo.pViewports = nullptr; //&viewport;
	viewportStateInfo.scissorCount = 1;
	viewportStateInfo.pScissors = nullptr; //&scissors;

										   // Configure rasterizer
	VkPipelineRasterizationStateCreateInfo rasterInfo{};
	rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterInfo.pNext = nullptr;
	rasterInfo.flags = 0;
	rasterInfo.depthClampEnable = VK_FALSE;
	rasterInfo.rasterizerDiscardEnable = VK_FALSE;
	//If we want to use different modes we have to enable the corresponding feature
	rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterInfo.depthBiasEnable = VK_FALSE;
	rasterInfo.depthBiasConstantFactor = 0.0f;
	rasterInfo.depthBiasClamp = 0.0f;
	rasterInfo.depthBiasSlopeFactor = 0.0f;
	rasterInfo.lineWidth = 1.0f;

	// Configure multi sampling state
	VkPipelineMultisampleStateCreateInfo multiSamplingInfo{};
	multiSamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multiSamplingInfo.pNext = nullptr;
	multiSamplingInfo.flags = 0;
	multiSamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multiSamplingInfo.sampleShadingEnable = VK_FALSE;
	multiSamplingInfo.minSampleShading = 1.0f;
	multiSamplingInfo.pSampleMask = nullptr;
	multiSamplingInfo.alphaToCoverageEnable = VK_FALSE;
	multiSamplingInfo.alphaToOneEnable = VK_FALSE;

	// Configure the Blend state
	VkPipelineColorBlendAttachmentState colorBlendState{};
	colorBlendState.blendEnable = VK_FALSE;
	colorBlendState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendState.alphaBlendOp = VK_BLEND_OP_ADD;
	colorBlendState.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
	colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendInfo.pNext = nullptr;
	colorBlendInfo.flags = 0;
	colorBlendInfo.logicOpEnable = VK_FALSE;
	colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendInfo.attachmentCount = 1;
	colorBlendInfo.pAttachments = &colorBlendState;
	colorBlendInfo.blendConstants[0] = 0.0f;
	colorBlendInfo.blendConstants[1] = 0.0f;
	colorBlendInfo.blendConstants[2] = 0.0f;
	colorBlendInfo.blendConstants[3] = 0.0f;

	// Define dynamic properties
	std::vector<VkDynamicState> dynamicStates{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.pNext = nullptr;
	dynamicStateInfo.flags = 0;
	dynamicStateInfo.dynamicStateCount = static_cast<u32>(dynamicStates.size());
	dynamicStateInfo.pDynamicStates = &dynamicStates[0];

	create_layout(RenderInterface::s_device);

	// Create the graphic pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;
	pipelineInfo.flags = 0;
	pipelineInfo.stageCount = static_cast<u32>(kStageCount);
	pipelineInfo.pStages = kStageInfo;
	pipelineInfo.pVertexInputState = &vertexShaderInfo;
	pipelineInfo.pInputAssemblyState = &assemblerInfo;
	pipelineInfo.pTessellationState = nullptr;
	pipelineInfo.pViewportState = &viewportStateInfo;
	pipelineInfo.pRasterizationState = &rasterInfo;
	pipelineInfo.pMultisampleState = &multiSamplingInfo;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlendInfo;
	pipelineInfo.pDynamicState = &dynamicStateInfo;
	pipelineInfo.layout = m_layout;
	pipelineInfo.renderPass = m_desc.m_pRenderPass->handle();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	auto result = vkCreateGraphicsPipelines(RenderInterface::s_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline);
	VK_CHECK(result, "Failed to create Graphics Pipeline");

	// Destroy modules now that are not needed
	destroy_shader_module(RenderInterface::s_device, vsMmodule);
	destroy_shader_module(RenderInterface::s_device, psMmodule);
#endif // DEAD
}

////////////////////////////////////////////////////////////////////////////////
// EOF