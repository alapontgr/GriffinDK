////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/17
//	File: 	GfRenderPass_Platform.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GfRenderPass.h"
#include "Common/GfRender/GfWindow.h"
#include "Common/GfRender/GfRenderContext.h"
#include "Common/GfRender/GfCmdBuffer.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfRenderPass)
	, m_pRenderPass(nullptr)
{
	for (u32 i = 0; i < GfRenderConstants::ms_uiNBufferingCount; i++) 
	{
		m_pFramebuffers[i] = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderPass_Platform::CreateRHI(const GfRenderContext& kCtx, const GfWindow* pWindow)
{
	// Description of the whole render pass
	VkAttachmentDescription kAttachmentsDesc{};
	kAttachmentsDesc.flags = 0;
	kAttachmentsDesc.format = pWindow->Plat().GetSwapchainFormat();
	kAttachmentsDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	kAttachmentsDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	kAttachmentsDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	kAttachmentsDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	kAttachmentsDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	kAttachmentsDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // The first frame the layout is not VK_IMAGE_LAYOUT_PRESENT_SRC_KHR. So using Undefined solves the problem with the validation layer
	kAttachmentsDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Reference for the color attachment of the sub pass 0
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Description of the sub pass 0
	VkSubpassDescription kSubPassDesc{};
	kSubPassDesc.flags = 0;
	kSubPassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	kSubPassDesc.inputAttachmentCount = 0;
	kSubPassDesc.pInputAttachments = nullptr;
	kSubPassDesc.colorAttachmentCount = 1;
	kSubPassDesc.pColorAttachments = &colorAttachmentRef;
	kSubPassDesc.pResolveAttachments = nullptr;
	kSubPassDesc.pDepthStencilAttachment = nullptr;
	kSubPassDesc.preserveAttachmentCount = 0;
	kSubPassDesc.pPreserveAttachments = nullptr;

	// Configure dependencies
	static constexpr u32 s_uiDependencyCount = 2;
	std::vector<VkSubpassDependency> tDependencies(s_uiDependencyCount);
	tDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	tDependencies[0].dstSubpass = 0; // Sub pass index
	tDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	tDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	tDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	tDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	tDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	tDependencies[1].srcSubpass = 0; // Sub pass index
	tDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	tDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	tDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	tDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	tDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	tDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


	// Create the render pass
	VkRenderPassCreateInfo kRenderPassInfo{};
	kRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	kRenderPassInfo.pNext = nullptr;
	kRenderPassInfo.flags = 0;
	kRenderPassInfo.attachmentCount = 1;
	kRenderPassInfo.pAttachments = &kAttachmentsDesc;
	kRenderPassInfo.subpassCount = 1;
	kRenderPassInfo.pSubpasses = &kSubPassDesc;
	kRenderPassInfo.dependencyCount = static_cast<u32>(tDependencies.size());
	kRenderPassInfo.pDependencies = &tDependencies[0];

	VkResult eResult = vkCreateRenderPass(kCtx.Plat().m_pDevice, &kRenderPassInfo, nullptr,
		&m_pRenderPass);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create render pass");
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderPass_Platform::BeginPassRHI(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfWindow* pWindow)
{
	// Begin render pass

	// TODO: Refactor this
	VkClearValue clearColor = { 1.0f, 0.0f, 0.0f, 0.0f };

	VkRenderPassBeginInfo passBeginInfo{};
	passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	passBeginInfo.pNext = nullptr;
	passBeginInfo.renderPass = m_pRenderPass;
	passBeginInfo.framebuffer = m_pFramebuffers[pWindow->GetCurrentFrameIdx()];
	passBeginInfo.renderArea.extent.width = pWindow->GetWidth();
	passBeginInfo.renderArea.extent.height = pWindow->GetHeight();
	passBeginInfo.renderArea.offset.x = 0;
	passBeginInfo.renderArea.offset.y = 0;
	passBeginInfo.clearValueCount = 1;
	passBeginInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(kCmdBuffer.Plat().GetCmdBuffer(), &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderPass_Platform::EndPassRHI(const GfCmdBuffer& kCmdBuffer)
{
	vkCmdEndRenderPass(kCmdBuffer.Plat().GetCmdBuffer());
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderPass_Platform::SetViewportRHI(const GfCmdBuffer& kCmdBuffer, const GfViewport& kViewport)
{
	VkViewport kRHIViewport{};
	kRHIViewport.x = kViewport.m_fOffsetX;
	kRHIViewport.y = kViewport.m_fOffsetY;
	kRHIViewport.width = kViewport.m_fWidth;
	kRHIViewport.height = kViewport.m_fHeight;
	kRHIViewport.minDepth = kViewport.m_fMinDepth;
	kRHIViewport.maxDepth = kViewport.m_fMaxDepth;
	vkCmdSetViewport(kCmdBuffer.Plat().GetCmdBuffer(), 0, 1, &kRHIViewport);
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderPass_Platform::SetScissorRHI(const GfCmdBuffer& kCmdBuffer, const GfScissor& kScissor)
{
	VkRect2D kRHIScissor{};
	kRHIScissor.offset.x = kScissor.m_siOffsetX;
	kRHIScissor.offset.y = kScissor.m_siOffsetY;
	kRHIScissor.extent.width = kScissor.m_siWidth;
	kRHIScissor.extent.height = kScissor.m_siHeight;
	vkCmdSetScissor(kCmdBuffer.Plat().GetCmdBuffer(), 0, 1, &kRHIScissor);
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderPass_Platform::RecreateFramebufferRHI(const GfRenderContext& kCtx, const GfWindow* pWindow)
{
	// Destroy the old one
	if (m_pFramebuffers)
	{
		for (u32 i = 0; i < GfRenderConstants::ms_uiNBufferingCount; i++) 
		{
			vkDestroyFramebuffer(kCtx.Plat().m_pDevice, m_pFramebuffers[i], nullptr);
			m_pFramebuffers[i] = VK_NULL_HANDLE;
		}
	}

	// TODO: Refactor this to use the attachments defined as output target parameters

	// REVISE
	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.pNext = nullptr;
	framebufferInfo.flags = 0;
	framebufferInfo.renderPass = m_pRenderPass;
	framebufferInfo.width = pWindow->GetWidth();
	framebufferInfo.height = pWindow->GetHeight();
	framebufferInfo.layers = 1;

	for (u32 i = 0; i < GfRenderConstants::ms_uiNBufferingCount; i++) 
	{
		VkImageView pOutView(pWindow->GetBackBufferView(i)->Plat().GetView());
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &pOutView;
		VkResult eResult = vkCreateFramebuffer(kCtx.Plat().m_pDevice, &framebufferInfo, VK_NULL_HANDLE, &m_pFramebuffers[i]);
		GF_ASSERT(eResult == VK_SUCCESS, "Failed to create framebuffer");
		//
	}

}

////////////////////////////////////////////////////////////////////////////////
// EOF