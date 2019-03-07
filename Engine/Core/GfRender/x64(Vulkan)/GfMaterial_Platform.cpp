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
#include "GfRender/Common/GfCmdBuffer.h"

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
	// IMPORTANT: pOutVertexAttributes must be a valid array of "kVertex.GetAttribCount()" elements. nullptr in case of 0 
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
	for (u32 i=0; i<EShaderStage::COUNT; ++i) 
	{
		if (m_kBase.m_pShaderStages[i].m_pModule) 
		{
			vkDestroyShaderModule(kCtx.m_pDevice, m_kBase.m_pShaderStages[i].m_pModule, nullptr);
			m_kBase.m_pShaderStages[i].m_pModule = VK_NULL_HANDLE;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

bool GfMaterialTemplate_Platform::CreateRHI(const GfRenderContext& kCtx)
{
	if (CreateLayout(kCtx) && CreatePipeline(kCtx))
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialTemplate_Platform::BindRHI(const GfCmdBuffer& kCmdBuffer)
{
	// TODO: Add support for Compute material
	vkCmdBindPipeline(kCmdBuffer.GetCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline());
}

////////////////////////////////////////////////////////////////////////////////

bool GfMaterialTemplate_Platform::CreateLayout(const GfRenderContext& kCtx)
{
	u32 uiLayoutCount(m_kBase.GetBoundLayoutCount());
	GfFrameMTStackAlloc::GfMemScope kMemScope(GfFrameMTStackAlloc::Get());
	VkDescriptorSetLayout* pLayouts(nullptr);
	if (uiLayoutCount)
	{
		pLayouts = GfFrameMTStackAlloc::Get()->Alloc<VkDescriptorSetLayout>(uiLayoutCount);
		
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
	if (siResult != VK_SUCCESS) 
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool GfMaterialTemplate_Platform::CreatePipeline(const GfRenderContext& kCtx)
{
	GfFrameMTStackAlloc::GfMemScope kMemScope(GfFrameMTStackAlloc::Get());

	// Define Shader modules
	VkPipelineShaderStageCreateInfo* pStages(GfFrameMTStackAlloc::Get()->Alloc<VkPipelineShaderStageCreateInfo>(EShaderStage::COUNT));
	if (!pStages) {return false; }

	u32 uiStageCount(0);
	for (u32 i=0; i<EShaderStage::COUNT; ++i) 
	{
		GfMaterialTemplate::GfShaderDesc& kStage(m_kBase.m_pShaderStages[i]);
		if (kStage.m_pSourceData && kStage.m_szEntryPoint)
		{
			if (!GfIsAligned(kStage.m_pSourceData, alignof(u32)))
			{
				return false;	
			}
			// Init Shader modules
			VkShaderModuleCreateInfo kModuleInfo;
			kModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			kModuleInfo.pNext = nullptr;
			kModuleInfo.flags = 0;
			kModuleInfo.codeSize = kStage.m_uiSrcDataSize;
			kModuleInfo.pCode = reinterpret_cast<const u32*>(kStage.m_pSourceData);

			VkResult siResult = vkCreateShaderModule(kCtx.m_pDevice, &kModuleInfo, nullptr, &kStage.m_pModule);
			if (siResult != VK_SUCCESS) 
			{
				return false;
			}

			// Assign the shader module to the stage
 			pStages[uiStageCount].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
 			pStages[uiStageCount].pNext = nullptr;
 			pStages[uiStageCount].flags = 0;
 			pStages[uiStageCount].stage = ConvertShaderStage((EShaderStage::Type)i);
 			pStages[uiStageCount].module = kStage.m_pModule;
 			pStages[uiStageCount].pName = kStage.m_szEntryPoint;
 			pStages[uiStageCount].pSpecializationInfo = nullptr;
 			uiStageCount++;
		}
	}

	// Parse input assembler format
	VkVertexInputBindingDescription kVertexBindingDesc{};
	u32 uiAttribCount(m_kBase.m_kVertexFormat.GetAttribCount());

	VkVertexInputAttributeDescription* pLayouts(nullptr);
	if (uiAttribCount) 
	{
		pLayouts = GfFrameMTStackAlloc::Get()->Alloc<VkVertexInputAttributeDescription>(uiAttribCount);
	}

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
	VkPipelineViewportStateCreateInfo kViewportStateInfo;
	kViewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	kViewportStateInfo.pNext = nullptr;
	kViewportStateInfo.flags = 0;
	kViewportStateInfo.viewportCount = 1;
	kViewportStateInfo.pViewports = nullptr; //&viewport;
	kViewportStateInfo.scissorCount = 1;
	kViewportStateInfo.pScissors = nullptr; //&scissors;
 
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
	// Shader stages
	kPipelineInfo.stageCount = uiStageCount;
	kPipelineInfo.pStages = pStages;
	// Input assembler
	kPipelineInfo.pVertexInputState = &kVertexShaderInfo;
	// Topology
	kPipelineInfo.pInputAssemblyState = &kAssemblerInfo;
	// Tessellation
	kPipelineInfo.pTessellationState = nullptr; // TODO: Add Tessellation support
	// Viewport and Scissors are set as dynamic
	kPipelineInfo.pViewportState = &kViewportStateInfo;
	// Raster state
	kPipelineInfo.pRasterizationState = &kRasterInfo;
	// MS State
	kPipelineInfo.pMultisampleState = &kMultiSamplingInfo;
	// Depth Stencil
	kPipelineInfo.pDepthStencilState = nullptr; // TODO: Add support for Depth/Stencil
	// Blend state
	kPipelineInfo.pColorBlendState = &kBlendCreateInfo;
	// Dynamic state
	kPipelineInfo.pDynamicState = &kDynStateInfo;

	kPipelineInfo.layout = m_pLayout;
	kPipelineInfo.renderPass = m_kBase.m_pMaterialPass->GetRenderPass();
	kPipelineInfo.subpass = 0;

	// TODO: Add material specialization (uber-shaders)
	kPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	kPipelineInfo.basePipelineIndex = -1;

	VkResult siResult = vkCreateGraphicsPipelines(kCtx.m_pDevice, VK_NULL_HANDLE, 1, &kPipelineInfo, nullptr, &m_pPipeline);
	if (siResult != VK_SUCCESS) 
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// EOF