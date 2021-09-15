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

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfRender/GfRenderPass.h"
#include "Common/GfRender/GfWindow.h"
#include "Common/GfRender/GfRenderContext.h"
#include "Common/GfRender/GfCmdBuffer.h"

////////////////////////////////////////////////////////////////////////////////
// Used for hashing

struct RenderPassDescHeader 
{
	u32 m_useDepth : 1;
	u32 m_outputCount : 31;
};
struct AttachmentDescHeader
{
	u32 m_format : 16;
	u32 m_loadOp : 4;
	u32 m_storeOp : 4;
	u32 m_stencilLoadOp : 4;
	u32 m_stencilStoreOp : 4;
};

////////////////////////////////////////////////////////////////////////////////

GfUMap<u64, VkRenderPass> GfRenderPass_Platform::ms_renderPassCache;
GfUMap<u64, VkFramebuffer> GfRenderPass_Platform::ms_framebufferCache;

VkAttachmentLoadOp convertLoadOp(LoadOp op) 
{
	static VkAttachmentLoadOp s_table[3] =
	{
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_LOAD_OP_DONT_CARE
	};
	return s_table[static_cast<u32>(op)];
}

static VkAttachmentStoreOp convertStoreOp(StoreOp op) 
{
	static VkAttachmentStoreOp s_table[3] =
	{
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_ATTACHMENT_STORE_OP_DONT_CARE
	};
	return s_table[static_cast<u32>(op)];
}

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfRenderPass)
	, m_framebuffer(nullptr)
	, m_renderPass(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderPass_Platform::getOrCreateRenderPass(const GfRenderContext& ctx)
{
	// If the hash is the same the RenderPass is the same
	if (m_renderPass == VK_NULL_HANDLE) 
	{
		GF_ASSERT_ALWAYS("TODO: Implement creation of RenderPass");	
	}
	if (m_framebuffer == VK_NULL_HANDLE) 
	{
		GF_ASSERT_ALWAYS("TODO: Implement creation of Framebuffer");
	}
}

void GfRenderPass_Platform::createRHI(const GfRenderContext& kCtx, const GfWindow* pWindow)
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
		&m_renderPass);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create render pass");
}

bool GfRenderPass_Platform::createRHI(const GfRenderContext& kCtx, 
	const AttachmentDesc* output, u32 outputCount, 
	const AttachmentDesc* depthAttachment)
{
	GF_ASSERT(outputCount > 0 || depthAttachment != nullptr, "Invalid numbr of color attachments");
	u64 hashRP(0);
	{
		u32 attachmentCount = outputCount + (depthAttachment ? 1 : 0);
		u32 reqSize = static_cast<u32>(sizeof(RenderPassDescHeader) + attachmentCount * sizeof(AttachmentDescHeader));
		StackMemBlock block(reqSize);
		RenderPassDescHeader* head = reinterpret_cast<RenderPassDescHeader*>(block.get());
		head->m_outputCount = outputCount;
		head->m_useDepth = depthAttachment ? 1 : 0;
		AttachmentDescHeader* attachments = reinterpret_cast<AttachmentDescHeader*>(head + 1);
		u32 pivot = 0;
		if (head->m_useDepth)
		{
			attachments[pivot].m_format = depthAttachment->m_attachment->getFormat();
			attachments[pivot].m_loadOp = static_cast<u32>(depthAttachment->m_loadOp);
			attachments[pivot].m_storeOp = static_cast<u32>(depthAttachment->m_storeOp);
			attachments[pivot].m_stencilLoadOp = static_cast<u32>(depthAttachment->m_stencilLoadOp);
			attachments[pivot].m_stencilStoreOp = static_cast<u32>(depthAttachment->m_stencilStoreOp);
			pivot++;
		}
		for (u32 i = 0; i < outputCount; ++i)
		{
			attachments[pivot].m_format = output[i].m_attachment->getFormat();
			attachments[pivot].m_loadOp = static_cast<u32>(output[i].m_loadOp);
			attachments[pivot].m_storeOp = static_cast<u32>(output[i].m_storeOp);
			attachments[pivot].m_stencilLoadOp = static_cast<u32>(output[i].m_stencilLoadOp);
			attachments[pivot].m_stencilStoreOp = static_cast<u32>(output[i].m_stencilStoreOp);
			pivot++;
		}
		// TODO: We can use the hash calculated in the base class
		hashRP = GfHash::compute(head, reqSize);
	}
	GF_ASSERT(hashRP, "Invalid hash");

	const auto entryRP = ms_renderPassCache.find(hashRP);
	if (entryRP != ms_renderPassCache.end()) 
	{
		m_renderPass = entryRP->second;
	}
	else 
	{
		m_renderPass = createRenderPass(kCtx, output, outputCount, depthAttachment);
		ms_renderPassCache[hashRP] = m_renderPass;
	}

	m_framebuffer = getOrCreateFramebuffer(kCtx, output, outputCount, depthAttachment);

	return true;
}

void GfRenderPass_Platform::markAsChanged()
{
	m_renderPass = VK_NULL_HANDLE;
	m_framebuffer = VK_NULL_HANDLE;
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
	passBeginInfo.renderPass = m_renderPass;
	passBeginInfo.framebuffer = m_framebuffer;
	passBeginInfo.renderArea.extent.width = pWindow->GetWidth();
	passBeginInfo.renderArea.extent.height = pWindow->GetHeight();
	passBeginInfo.renderArea.offset.x = 0;
	passBeginInfo.renderArea.offset.y = 0;
	passBeginInfo.clearValueCount = 1;
	passBeginInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(kCmdBuffer.Plat().getCmdBuffer(), &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderPass_Platform::EndPassRHI(const GfCmdBuffer& kCmdBuffer)
{
	vkCmdEndRenderPass(kCmdBuffer.Plat().getCmdBuffer());
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
	vkCmdSetViewport(kCmdBuffer.Plat().getCmdBuffer(), 0, 1, &kRHIViewport);
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderPass_Platform::SetScissorRHI(const GfCmdBuffer& kCmdBuffer, const GfScissor& kScissor)
{
	VkRect2D kRHIScissor{};
	kRHIScissor.offset.x = kScissor.m_siOffsetX;
	kRHIScissor.offset.y = kScissor.m_siOffsetY;
	kRHIScissor.extent.width = kScissor.m_siWidth;
	kRHIScissor.extent.height = kScissor.m_siHeight;
	vkCmdSetScissor(kCmdBuffer.Plat().getCmdBuffer(), 0, 1, &kRHIScissor);
}

VkRenderPass GfRenderPass_Platform::createRenderPass(const GfRenderContext& kCtx, 
	const AttachmentDesc* output, u32 outputCount, 
	const AttachmentDesc* depthAttachment)
{
	bool useDepth = (depthAttachment != nullptr);
	u32 attachmentCount = outputCount + (useDepth ? 1 : 0);

	StackMemBlock attachmentsDescs(static_cast<u32>(sizeof(VkAttachmentDescription)) * attachmentCount);
	StackMemBlock attachmentsRefs(static_cast<u32>(sizeof(VkAttachmentReference)) * attachmentCount);
	
	VkAttachmentDescription* attachments = reinterpret_cast<VkAttachmentDescription*>(attachmentsDescs.get());
	VkAttachmentReference* attachmentReferences = reinterpret_cast<VkAttachmentReference*>(attachmentsRefs.get());
	
	u32 attachmentIdx = 0;
	VkAttachmentDescription* depthAttachmentDesc = attachments;
	VkAttachmentReference* depthAttachmentRef = attachmentReferences;
	if (useDepth) 
	{
		depthAttachmentDesc->flags = 0;
		depthAttachmentDesc->format = ConvertTextureFormat(depthAttachment->m_attachment->getFormat());
		depthAttachmentDesc->samples = VkSampleCountFlagBits ::VK_SAMPLE_COUNT_1_BIT;
		depthAttachmentDesc->loadOp = convertLoadOp(depthAttachment->m_loadOp);
		depthAttachmentDesc->storeOp = convertStoreOp(depthAttachment->m_storeOp);
		depthAttachmentDesc->stencilLoadOp = convertLoadOp(depthAttachment->m_stencilLoadOp);
		depthAttachmentDesc->stencilStoreOp = convertStoreOp(depthAttachment->m_stencilStoreOp);
		depthAttachmentDesc->initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachmentDesc->finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		depthAttachmentRef->attachment = attachmentIdx++;
		depthAttachmentRef->layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	VkAttachmentDescription* outputAttachmentDescs = depthAttachmentDesc+1;
	VkAttachmentReference* outputRefs = depthAttachmentRef+1;
	for (u32 i=0; i<outputCount; ++i) 
	{
		outputAttachmentDescs[i].flags = 0;
		outputAttachmentDescs[i].format = ConvertTextureFormat(output[i].m_attachment->getFormat());
		outputAttachmentDescs[i].samples = VkSampleCountFlagBits ::VK_SAMPLE_COUNT_1_BIT;
		outputAttachmentDescs[i].loadOp = convertLoadOp(output[i].m_loadOp);
		outputAttachmentDescs[i].storeOp = convertStoreOp(output[i].m_storeOp);
		outputAttachmentDescs[i].stencilLoadOp = convertLoadOp(output[i].m_stencilLoadOp);
		outputAttachmentDescs[i].stencilStoreOp = convertStoreOp(output[i].m_stencilStoreOp);
		outputAttachmentDescs[i].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		outputAttachmentDescs[i].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		outputRefs[i].attachment = attachmentIdx++;
		outputRefs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	VkSubpassDescription subpassDesc{};
	subpassDesc.flags = 0;
	subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDesc.inputAttachmentCount = 0;
	subpassDesc.pInputAttachments = nullptr;
	subpassDesc.colorAttachmentCount = outputCount;
	subpassDesc.pColorAttachments = outputCount > 0 ? outputRefs : nullptr;
	subpassDesc.pResolveAttachments = nullptr; // TODO
	subpassDesc.pDepthStencilAttachment = useDepth ? attachmentReferences : nullptr;
	subpassDesc.preserveAttachmentCount = 0; // TODO
	subpassDesc.pPreserveAttachments = nullptr; // TODO

	VkRenderPassCreateInfo kRenderPassInfo{};
	kRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	kRenderPassInfo.pNext = nullptr;
	kRenderPassInfo.flags = 0;
	kRenderPassInfo.attachmentCount = attachmentCount;
	kRenderPassInfo.pAttachments = attachments;
	kRenderPassInfo.subpassCount = 1;
	kRenderPassInfo.pSubpasses = &subpassDesc;
	kRenderPassInfo.dependencyCount = 0;
	kRenderPassInfo.pDependencies = nullptr;

	VkRenderPass renderPass(nullptr);
	VkResult eResult = vkCreateRenderPass(kCtx.Plat().m_pDevice, &kRenderPassInfo, nullptr, &renderPass);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create render pass");
	return renderPass;
}

VkFramebuffer GfRenderPass_Platform::getOrCreateFramebuffer(const GfRenderContext& ctx, const AttachmentDesc* output, u32 outputCount, const AttachmentDesc* depthAttachment)
{
	bool useDepth = (depthAttachment != nullptr);
	u32 attachmentCount = outputCount + (useDepth ? 1 : 0);

	u32 reqSize = static_cast<u32>(sizeof(VkFramebufferCreateInfo)) + static_cast<u32>(sizeof(VkImageView)) * attachmentCount;
	StackMemBlock frameBufferDescription(reqSize);

	u32 width = 0;
	u32 height = 0;
	if (outputCount > 0) 
	{
		width = output[0].m_attachment->getWidth();
		height = output[0].m_attachment->getHeight();
	}
	else 
	{
		width = depthAttachment->m_attachment->getWidth();
		height = depthAttachment->m_attachment->getHeight();
	}

	VkFramebufferCreateInfo* frameBufferCI = reinterpret_cast<VkFramebufferCreateInfo*>(frameBufferDescription.get());
	frameBufferCI->sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCI->pNext = nullptr;
	frameBufferCI->flags = 0;
	frameBufferCI->renderPass = nullptr;
	frameBufferCI->pAttachments = nullptr;
	frameBufferCI->layers = 1;
	frameBufferCI->width = width;
	frameBufferCI->height = height;
	VkImageView* attachmentViews = reinterpret_cast<VkImageView*>(frameBufferCI+1);
	u32 idx = 0;
	if (useDepth) 
	{
		attachmentViews[idx++] = depthAttachment->m_attachment->Plat().getView();
	}
	for (u32 i = 0; i < outputCount; ++i) 
	{
		attachmentViews[idx++] = output[i].m_attachment->Plat().getView();
	}
	u64 fbHash = GfHash::compute(frameBufferCI, reqSize);

	const auto entry = ms_framebufferCache.find(fbHash);
	VkFramebuffer fb = nullptr;
	if (entry != ms_framebufferCache.end()) 
	{
		fb = entry->second;
	}
	else 
	{
		frameBufferCI->attachmentCount = attachmentCount;
		frameBufferCI->pAttachments = attachmentViews;
		frameBufferCI->renderPass = m_renderPass;
		VkResult eResult = vkCreateFramebuffer(ctx.Plat().m_pDevice, frameBufferCI, VK_NULL_HANDLE, &fb);
		GF_ASSERT(eResult == VK_SUCCESS, "Failed to create framebuffer");
		ms_framebufferCache[fbHash] = fb;
	}
	return fb;
}

////////////////////////////////////////////////////////////////////////////////
// EOF