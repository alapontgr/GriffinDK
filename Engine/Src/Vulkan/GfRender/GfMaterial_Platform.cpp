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

#include "Common/GfRender/GfMaterial.h"
#include "Common/GfRender/GfMatParamLayout.h"
#include "Common/GfRender/GfRenderContext.h"
#include "Common/Gfrender/GfRenderPass.h"
#include "Common/GfRender/GfCmdBuffer.h"

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

static inline void ConvertInputVertex(const GfVertexDeclaration& kVertex, 
	VkVertexInputBindingDescription* outVertexBindings,
	VkVertexInputAttributeDescription* pOutVertexAttributes)
{
	static VkVertexInputRate s_vertexRateTable[] = {VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX, VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE};

	for (const GfVertexDeclaration::VertexBufferBinding& vertexBuffer : kVertex.getVertexBuffers()) 
	{
		outVertexBindings->binding = vertexBuffer.m_bufferIdx;
		outVertexBindings->inputRate = s_vertexRateTable[vertexBuffer.m_rate]; // Values are equivalent in Vulkan enum
		outVertexBindings->stride = vertexBuffer.m_stride;
		outVertexBindings++;
	}

	for (const GfVertexDeclaration::AttributeDesc& attribute : kVertex.getAttributes()) 
	{
		pOutVertexAttributes->offset = attribute.m_uiOffset;
		pOutVertexAttributes->location = attribute.m_location;
		pOutVertexAttributes->binding = attribute.m_vertexBufferIdx;
		pOutVertexAttributes->format = ConvertAttributeFormat(attribute.m_eType);
		pOutVertexAttributes++;
	}
}

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfMaterialTemplate)
	, m_pLayout(nullptr)
	, m_pPipeline(nullptr)
{

}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialTemplate_Platform::destroyRHI(const GfRenderContext& kCtx)
{
	if (m_pPipeline)
	{
		vkDestroyPipeline(kCtx.Plat().m_pDevice, m_pPipeline, nullptr);
		m_pPipeline = nullptr;
	}
	if (m_pLayout) 
	{
		vkDestroyPipelineLayout(kCtx.Plat().m_pDevice, m_pLayout, nullptr);
		m_pLayout = nullptr;
	}
	for (u32 i=0; i<EShaderStage::Count; ++i) 
	{
		if (m_kBase.m_pShaderStages[i].m_pModule) 
		{
			vkDestroyShaderModule(kCtx.Plat().m_pDevice, m_kBase.m_pShaderStages[i].m_pModule, nullptr);
			m_kBase.m_pShaderStages[i].m_pModule = VK_NULL_HANDLE;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

bool GfMaterialTemplate_Platform::createRHI(const GfRenderContext& kCtx)
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
	vkCmdBindPipeline(kCmdBuffer.Plat().getCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, GetPipeline());
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialTemplate_Platform::PushConstantsRHI(const GfCmdBuffer& kCmdBuffer, u32 uiSize, void* pData)
{
	vkCmdPushConstants(kCmdBuffer.Plat().getCmdBuffer(), m_pLayout, m_uiPushConstantsStage, 0, uiSize, pData);
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
			const GfMatParamLayout* pLayout(m_kBase.m_pLayouts[i]);
			if (pLayout)
			{
				GF_ASSERT(pLayout->IsGPUInitialised(), "The layout needs to be initialised before the creation of the material");
				*pCursor = pLayout->Plat().GetLayout();
				pCursor++;
			}
		}
	}

	VkPushConstantRange kPossibleRange;
	VkPipelineLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.pNext = nullptr;
	layoutInfo.flags = 0;
	layoutInfo.setLayoutCount = uiLayoutCount;
	layoutInfo.pSetLayouts = pLayouts;
	if (m_kBase.m_uiConstantsBlockSize != 0) 
	{
		m_uiPushConstantsStage = ConvertShaderStageFlags(m_kBase.m_uiConstantsStages);
		// TODO: Any case we may need more ranges?
		kPossibleRange.stageFlags = m_uiPushConstantsStage;
		kPossibleRange.offset = 0;
		kPossibleRange.size = m_kBase.m_uiConstantsBlockSize;
		layoutInfo.pushConstantRangeCount = 1;
		layoutInfo.pPushConstantRanges = &kPossibleRange;
	}
	else 
	{
		m_uiPushConstantsStage = 0;
		layoutInfo.pushConstantRangeCount = 0;
		layoutInfo.pPushConstantRanges = nullptr;
	}
	VkResult siResult = vkCreatePipelineLayout(kCtx.Plat().m_pDevice, &layoutInfo, nullptr, &m_pLayout);
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
	VkPipelineShaderStageCreateInfo* pStages(GfFrameMTStackAlloc::Get()->Alloc<VkPipelineShaderStageCreateInfo>(EShaderStage::Count));
	if (!pStages) {return false; }

	u32 uiStageCount(0);
	for (u32 i=0; i<EShaderStage::Count; ++i) 
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

			VkResult siResult = vkCreateShaderModule(kCtx.Plat().m_pDevice, &kModuleInfo, nullptr, &kStage.m_pModule);
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
	u32 attribCount(static_cast<u32>(m_kBase.m_kVertexFormat.getAttributes().size()));
	u32 vertexBufferCount(static_cast<u32>(m_kBase.m_kVertexFormat.getVertexBuffers().size()));

	GF_VERIFY(attribCount > 0 && vertexBufferCount > 0, "Invalid vertex description");
	VkVertexInputBindingDescription* vertexBindingDescs(GfFrameMTStackAlloc::Get()->Alloc<VkVertexInputBindingDescription>(vertexBufferCount));
	VkVertexInputAttributeDescription* attributes(GfFrameMTStackAlloc::Get()->Alloc<VkVertexInputAttributeDescription>(attribCount));

	ConvertInputVertex(m_kBase.m_kVertexFormat, vertexBindingDescs, attributes);
	
	VkPipelineVertexInputStateCreateInfo kVertexShaderInfo{};
	kVertexShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	kVertexShaderInfo.pNext = nullptr;
	kVertexShaderInfo.flags = 0;
	kVertexShaderInfo.vertexBindingDescriptionCount = vertexBufferCount;
	kVertexShaderInfo.pVertexBindingDescriptions = vertexBindingDescs;
	kVertexShaderInfo.vertexAttributeDescriptionCount = attribCount;
	kVertexShaderInfo.pVertexAttributeDescriptions = attributes;

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
	kPipelineInfo.renderPass = m_kBase.m_pMaterialPass->Plat().getRenderPass();
	kPipelineInfo.subpass = 0;

	// TODO: Add material specialization (uber-shaders)
	kPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	kPipelineInfo.basePipelineIndex = -1;

	VkResult siResult = vkCreateGraphicsPipelines(kCtx.Plat().m_pDevice, VK_NULL_HANDLE, 1, &kPipelineInfo, nullptr, &m_pPipeline);
	if (siResult != VK_SUCCESS) 
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// EOF