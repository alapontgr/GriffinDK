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

GfMutex GfRenderPass_Platform::ms_renderPasscacheMutex;
GfMutex GfRenderPass_Platform::ms_framebuffercacheMutex;
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
		u64 hash = m_kBase.getHash();

		GfLock<GfMutex> lock(ms_renderPasscacheMutex);
		const auto entryRP = ms_renderPassCache.find(hash);
		if (entryRP != ms_renderPassCache.end()) 
		{
			m_renderPass = entryRP->second;
		}
		else 
		{
			m_renderPass = createRenderPass(ctx);
			ms_renderPassCache[hash] = m_renderPass;
		}
	}
	if (m_framebuffer == VK_NULL_HANDLE) 
	{
		m_framebuffer = getCreateFramebuffer(ctx);
	}
}

void GfRenderPass_Platform::markAsChanged()
{
	m_renderPass = VK_NULL_HANDLE;
	m_framebuffer = VK_NULL_HANDLE;
}


VkRenderPass GfRenderPass_Platform::createRenderPass(const GfRenderContext& ctx)
{	
	// Do a second safe search in case another thread was previously creating this RenderPass
	const auto entryRP = ms_renderPassCache.find(m_kBase.getHash());
	if (entryRP != ms_renderPassCache.end()) 
	{
		return entryRP->second;
	}

	bool useDepth = m_kBase.usesDepthAttachment();
	u32 attachmentCount = m_kBase.m_outputCount + (useDepth ? 1 : 0);

	StackMemBlock attachmentsDescs(static_cast<u32>(sizeof(VkAttachmentDescription)) * attachmentCount);
	StackMemBlock attachmentsRefs(static_cast<u32>(sizeof(VkAttachmentReference)) * attachmentCount);
	
	VkAttachmentDescription* attachments = reinterpret_cast<VkAttachmentDescription*>(attachmentsDescs.get());
	VkAttachmentReference* attachmentReferences = reinterpret_cast<VkAttachmentReference*>(attachmentsRefs.get());
	
	u32 attachmentIdx = 0;
	VkAttachmentDescription* depthAttachmentDesc = attachments;
	VkAttachmentReference* depthAttachmentRef = attachmentReferences;
	if (useDepth) 
	{
		const AttachmentDesc* depthAttachment = &m_kBase.m_depthAttachment;
		depthAttachmentDesc->flags = 0;
		depthAttachmentDesc->format = ConvertTextureFormat(depthAttachment->m_attachment->getTexture()->getFormat());
		depthAttachmentDesc->samples = VkSampleCountFlagBits ::VK_SAMPLE_COUNT_1_BIT; // TODO: MSAA
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
	const AttachmentDesc* colorAttachments = m_kBase.m_attachments.data();
	for (u32 i=0; i<m_kBase.m_outputCount; ++i) 
	{
		outputAttachmentDescs[i].flags = 0;
		outputAttachmentDescs[i].format = ConvertTextureFormat(colorAttachments[i].m_attachment->getTexture()->getFormat());
		outputAttachmentDescs[i].samples = VkSampleCountFlagBits ::VK_SAMPLE_COUNT_1_BIT; // TODO: MSAA
		outputAttachmentDescs[i].loadOp = convertLoadOp(colorAttachments[i].m_loadOp);
		outputAttachmentDescs[i].storeOp = convertStoreOp(colorAttachments[i].m_storeOp);
		outputAttachmentDescs[i].stencilLoadOp = convertLoadOp(colorAttachments[i].m_stencilLoadOp);
		outputAttachmentDescs[i].stencilStoreOp = convertStoreOp(colorAttachments[i].m_stencilStoreOp);
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
	subpassDesc.colorAttachmentCount = m_kBase.m_outputCount;
	subpassDesc.pColorAttachments = m_kBase.m_outputCount > 0 ? outputRefs : nullptr;
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
	VkResult eResult = vkCreateRenderPass(ctx.Plat().m_pDevice, &kRenderPassInfo, nullptr, &renderPass);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create render pass");
	return renderPass;
}

VkFramebuffer GfRenderPass_Platform::getCreateFramebuffer(const GfRenderContext& ctx)
{
	AttachmentDesc* depthAttachment = &m_kBase.m_depthAttachment;
	AttachmentDesc* colorAttachments = m_kBase.m_attachments.data();

	bool useDepth = m_kBase.usesDepthAttachment();
	u32 attachmentCount = m_kBase.m_outputCount + (useDepth ? 1 : 0);

	u32 reqSize = static_cast<u32>(sizeof(VkFramebufferCreateInfo)) + static_cast<u32>(sizeof(VkImageView)) * attachmentCount;
	StackMemBlock frameBufferDescription(reqSize);

	u32 width = m_kBase.getWidth();
	u32 height = m_kBase.getHeight();

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
		attachmentViews[idx++] = reinterpret_cast<VkImageView>(depthAttachment->m_attachment->getViewID(ctx));
	}
	for (u32 i = 0; i < m_kBase.m_outputCount; ++i) 
	{
		attachmentViews[idx++] = reinterpret_cast<VkImageView>(colorAttachments[i].m_attachment->getViewID(ctx));
	}
	u64 fbHash = GfHash::compute(frameBufferCI, reqSize);

	VkFramebuffer fb = nullptr;
	{
		GfLock<GfMutex> lock(ms_framebuffercacheMutex);
		const auto entry = ms_framebufferCache.find(fbHash);
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
	}
	return fb;
}

////////////////////////////////////////////////////////////////////////////////
// EOF