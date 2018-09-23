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

#include "GfRender/Common/GfRenderPass.h"
#include "GfRender/Common/GfRenderContext.h"
#include "GfRender/Common/GfWindow.h"

////////////////////////////////////////////////////////////////////////////////

GfRenderPass_Platform::GfRenderPass_Platform()
	: m_pRenderPass(nullptr)
	, m_pFramebuffer(nullptr)
{

}

////////////////////////////////////////////////////////////////////////////////

void GfRenderPass_Platform::CompilePlatform(const GfRenderContext& kCtx)
{
	// Description of the whole render pass
	VkAttachmentDescription kAttachmentsDesc{};
	kAttachmentsDesc.flags = 0;
	kAttachmentsDesc.format = kCtx.GetSwapchainFormat();
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

	VkResult eResult = vkCreateRenderPass(kCtx.m_pDevice, &kRenderPassInfo, nullptr,
		&m_pRenderPass);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create render pass");
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderPass_Platform::RecreateFramebuffer(const GfRenderContext& kCtx)
{
	// Destroy the old one
	if (m_pFramebuffer)
	{
		vkDestroyFramebuffer(kCtx.m_pDevice, m_pFramebuffer, nullptr);
		m_pFramebuffer = VK_NULL_HANDLE;
	}

	// TODO: Refactor this to use the attachments defined as output target parameters

	GfWindow* pWindow(kCtx.GetWindow());
	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.pNext = nullptr;
	framebufferInfo.flags = 0;
	framebufferInfo.renderPass = m_pRenderPass;
	framebufferInfo.width = pWindow->GetWidth();
	framebufferInfo.height = pWindow->GetHeight();
	framebufferInfo.layers = 1;
	// REVISE
	VkImageView pOutView(kCtx.GetCurrentBackBufferView());
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = &pOutView;
	//

	VkResult eResult = vkCreateFramebuffer(kCtx.m_pDevice, &framebufferInfo, VK_NULL_HANDLE, &m_pFramebuffer);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create framebuffer");
}

////////////////////////////////////////////////////////////////////////////////
// EOF