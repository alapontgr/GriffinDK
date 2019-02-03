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
#include "Gfrender/Common/GfRenderPass.h"

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

static inline void ConvertBlendState(const GfBlendState& kBlendState,
	VkPipelineColorBlendAttachmentState& kBlendAttachment, VkPipelineColorBlendStateCreateInfo& kOut)
{
	kBlendAttachment.blendEnable = kBlendState.m_bEnabled ? VK_TRUE : VK_FALSE;
	kBlendAttachment.srcColorBlendFactor = ConvertBlendFactor(kBlendState.m_eSrcColorBlendFactor);
	kBlendAttachment.dstColorBlendFactor = ConvertBlendFactor(kBlendState.m_eDstColorBlendFactor);
	kBlendAttachment.colorBlendOp = ConvertBlendOp(kBlendState.m_eColorBlendOp);
	kBlendAttachment.srcAlphaBlendFactor = ConvertBlendFactor(kBlendState.m_eSrcAlphaBlendFactor);
	kBlendAttachment.dstAlphaBlendFactor = ConvertBlendFactor(kBlendState.m_eDstAlphaBlendFactor);
	kBlendAttachment.alphaBlendOp = ConvertBlendOp(kBlendState.m_eAlphaBlendOp);;
	kBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;

	kOut.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	kOut.pNext = nullptr;
	kOut.flags = 0;
	kOut.logicOpEnable = kBlendState.m_bLogicOpEnabled ? VK_TRUE : VK_FALSE;
	kOut.logicOp = ConvertBlendLogicOp(kBlendState.m_eBlendLogicOp);
	kOut.attachmentCount = 1;
	kOut.pAttachments = &kBlendAttachment;
	kOut.blendConstants[0] = kBlendState.m_vBlendConstants.x;
	kOut.blendConstants[1] = kBlendState.m_vBlendConstants.y;
	kOut.blendConstants[2] = kBlendState.m_vBlendConstants.z;
	kOut.blendConstants[3] = kBlendState.m_vBlendConstants.w;
}

////////////////////////////////////////////////////////////////////////////////

static inline void ConvertInputAssembler(const EPrimitiveTopology::Type eTopology, VkPipelineInputAssemblyStateCreateInfo& kOut) 
{
	kOut.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	kOut.pNext = nullptr;
	kOut.flags = 0;
	kOut.topology = ConvertTopology(eTopology);
	kOut.primitiveRestartEnable = VK_FALSE; // TODO
}

////////////////////////////////////////////////////////////////////////////////

static inline void ConvertInputVertex(const GfVertexDeclaration& kVertex, u32 uiBinding,
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
		GfVertexDeclaration::GfVertexAttrib kAttrib(kVertex.GetAttrib(i));
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
	GfFrameMTStackAlloc::GfMemScope kMemScope(GfFrameMTStackAlloc::Get());

	// TODO: Define Shader modules

	// Parse input assembler format
	VkVertexInputBindingDescription kVertexBindingDesc{};
	u32 uiAttribCount(m_kBase.m_kVertexFormat.GetAttribCount());
	VkVertexInputAttributeDescription* pLayouts(GfFrameMTStackAlloc::Get()->Alloc<VkVertexInputAttributeDescription>(uiAttribCount));
	ConvertInputVertex(m_kBase.m_kVertexFormat, 0, kVertexBindingDesc, pLayouts);
	
	VkPipelineVertexInputStateCreateInfo kVertexShaderInfo{};
	kVertexShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	kVertexShaderInfo.pNext = nullptr;
	kVertexShaderInfo.flags = 0;
	kVertexShaderInfo.vertexBindingDescriptionCount = 1; // TODO: Add support for several vertex bindings
	kVertexShaderInfo.pVertexBindingDescriptions = &kVertexBindingDesc;
	kVertexShaderInfo.vertexAttributeDescriptionCount = uiAttribCount;
	kVertexShaderInfo.pVertexAttributeDescriptions = pLayouts;

	// Define topology to feed the shaders
	VkPipelineInputAssemblyStateCreateInfo kAssemblerInfo{};
	ConvertInputAssembler(m_kBase.m_eTopology, kAssemblerInfo);

	// Define default viewport and scissors
	VkPipelineViewportStateCreateInfo viewportStateInfo;
	viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfo.pNext = nullptr;
	viewportStateInfo.flags = 0;
	viewportStateInfo.viewportCount = 1;
	viewportStateInfo.pViewports = nullptr; //&viewport;
	viewportStateInfo.scissorCount = 1;
	viewportStateInfo.pScissors = nullptr; //&scissors;
 
 	// Configure rasterizer
 	VkPipelineRasterizationStateCreateInfo kRasterInfo{};
 	ConvertRasterState(m_kBase.m_kRasterState, kRasterInfo);
 
 	// Configure multi sampling state
 	VkPipelineMultisampleStateCreateInfo kMultiSamplingInfo{};
 	ConvertMSState(m_kBase.m_kMSState, kMultiSamplingInfo);
 
 	// Convert BlendState
	VkPipelineColorBlendAttachmentState kBlendAttachment{};
	VkPipelineColorBlendStateCreateInfo kBlendCreateInfo{};
	ConvertBlendState(m_kBase.m_kBlendState, kBlendAttachment, kBlendCreateInfo);

	// Define dynamic properties
	VkDynamicState pDynamicStates[] = 
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR 
	};
	VkPipelineDynamicStateCreateInfo kDynStateInfo{};
	kDynStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	kDynStateInfo.pNext = nullptr;
	kDynStateInfo.flags = 0;
	kDynStateInfo.dynamicStateCount = (sizeof(pDynamicStates) / sizeof(pDynamicStates[0]));
	kDynStateInfo.pDynamicStates = pDynamicStates;
		
	// Create the graphics pipeline
	VkGraphicsPipelineCreateInfo kPipelineInfo{};
	kPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	kPipelineInfo.pNext = nullptr;
	kPipelineInfo.flags = 0;
	// TODO: Deal with stage definition
	// 	kPipelineInfo.stageCount = static_cast<u32>(kStageCount);
	// 	kPipelineInfo.pStages = kStageInfo;
	kPipelineInfo.pVertexInputState = &kVertexShaderInfo;
	kPipelineInfo.pInputAssemblyState = &kAssemblerInfo;
	kPipelineInfo.pTessellationState = nullptr;
	kPipelineInfo.pViewportState = &viewportStateInfo;
	kPipelineInfo.pRasterizationState = &kRasterInfo;
	kPipelineInfo.pMultisampleState = &kMultiSamplingInfo;
	kPipelineInfo.pDepthStencilState = nullptr;
	kPipelineInfo.pColorBlendState = &kBlendCreateInfo;
	kPipelineInfo.pDynamicState = &kDynStateInfo;
	kPipelineInfo.layout = m_pLayout;
	kPipelineInfo.renderPass = m_kBase.m_pMaterialPass->GetRenderPass();
	kPipelineInfo.subpass = 0;
	kPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	kPipelineInfo.basePipelineIndex = -1;

	VkResult siResult = vkCreateGraphicsPipelines(kCtx.m_pDevice, VK_NULL_HANDLE, 1, &kPipelineInfo, nullptr, &m_pPipeline);
	GF_ASSERT(siResult == VK_SUCCESS, "Failed to create Graphics Pipeline");

}

////////////////////////////////////////////////////////////////////////////////
// EOF