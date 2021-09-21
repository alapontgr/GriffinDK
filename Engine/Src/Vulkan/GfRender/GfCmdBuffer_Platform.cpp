////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/16
//	File: 	GFCMDBUFFER_PLATFORM.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GfCmdBuffer.h"
#include "Common/GfRender/GfRenderContext.h"
#include "Common/GfRender/GfWindow.h"
#include "Common/GfRender/GfRenderPass.h"
#include "Common/GfRender/GfShaderPipeline.h"
#include "Common/GfRender/GfMaterial.h"
#include "Common/GfRender/GfMatParamLayout.h"
#include "GfCmdBuffer_Platform.h"

////////////////////////////////////////////////////////////////////////////////

#define GF_INFINITE_TIMEOUT 1000000000

////////////////////////////////////////////////////////////////////////////////

GfCmdBufferFactory_Platform::GfCmdBufferFactory_Platform()
	: m_pool(VK_NULL_HANDLE)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBufferFactory_Platform::initRHI(const GfRenderContext& kCtx, GfRenderContextFamilies::Type queueType)
{
	VkCommandPoolCreateInfo kInfo{};
	kInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	kInfo.pNext = nullptr;
	kInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	kInfo.queueFamilyIndex = kCtx.GetFamilyIdx(queueType);
	VkResult eResult = vkCreateCommandPool(kCtx.Plat().m_pDevice, &kInfo, nullptr, &m_pool);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create command pool");
}

void GfCmdBufferFactory_Platform::shutdown(const GfRenderContext& ctx)
{
	if (m_pool) 
	{
		vkDestroyCommandPool(ctx.Plat().m_pDevice, m_pool, nullptr);
		m_pool = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

GfCmdBuffer_Platform::GfCmdBuffer_Platform()
	: m_cmdBuffer(nullptr)
	, m_fence(0)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::initRHI (const GfRenderContext& ctx, const GfCmdBufferFactory& factory, u32 type)
{
	GF_ASSERT(!m_cmdBuffer && !m_fence, "CMDBuffer already initialised");
	bool bIsPrimary = (type == GfCmdBufferType::Primary);
	VkCommandBufferAllocateInfo kinfo;
	kinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	kinfo.pNext = nullptr;
	kinfo.commandPool = factory.Plat().getPool();
	kinfo.level = bIsPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	kinfo.commandBufferCount = 1;

	VkResult eResult = vkAllocateCommandBuffers(ctx.Plat().m_pDevice, &kinfo, &m_cmdBuffer);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to allocate command buffers");

	// Create the fences for the command buffers
	VkFenceCreateInfo kFenceInfo;
	kFenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	kFenceInfo.pNext = nullptr;
	kFenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Signaled by default

	eResult = vkCreateFence(ctx.Plat().m_pDevice, &kFenceInfo, nullptr, &m_fence);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create a fence");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::waitForReadyRHI(const GfRenderContext& kCtx)
{
	VkResult eResult = vkWaitForFences(kCtx.Plat().m_pDevice, 1, &m_fence, VK_FALSE, GF_INFINITE_TIMEOUT);
	GF_ASSERT(eResult == VK_SUCCESS, "Waited to long for fences");
	eResult = vkResetFences(kCtx.Plat().m_pDevice, 1, &m_fence);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to reset the fences");
}

bool GfCmdBuffer_Platform::isReady(const GfRenderContext& kCtx)
{
	VkResult res = vkGetFenceStatus(kCtx.Plat().m_pDevice, m_fence);
	return res == VK_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::submitRHI(
	const GfRenderContext& kCtx,
	const GfWindow& kWindow,
	GfRenderContextFamilies::Type eQueueType,
	Bool bLast)
{
	const GfFrameSyncing& kSyncPrimitives(kWindow.Plat().GetFrameSyncPrimitives());
	VkPipelineStageFlags uiFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo kInfo{};
	kInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	kInfo.pNext = nullptr;
	if (bLast) 
	{
		kInfo.waitSemaphoreCount = 1;
		kInfo.pWaitSemaphores = &kSyncPrimitives.m_pImageReady;
		kInfo.pWaitDstStageMask = &uiFlags;
		kInfo.signalSemaphoreCount = 1;
		kInfo.pSignalSemaphores = &kSyncPrimitives.m_pFinishedRendering;
	}
	kInfo.commandBufferCount = 1;
	kInfo.pCommandBuffers = &m_cmdBuffer;

	VkResult eResult = vkQueueSubmit(kCtx.Plat().GetQueue(eQueueType), 1, &kInfo, m_fence);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to submit cmd block");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::beginRecordingRHI(const GfRenderContext& kCtx)
{
	VkCommandBufferBeginInfo kInfo;
	kInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	kInfo.pNext = nullptr;
	kInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	kInfo.pInheritanceInfo = nullptr;

	VkImageSubresourceRange range;
	range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	range.baseMipLevel = 0;
	range.levelCount = 1;
	range.baseArrayLayer = 0;
	range.layerCount = 1;

	// begin command buffer
	VkResult eResult = vkBeginCommandBuffer(m_cmdBuffer, &kInfo);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to begin cmd buffer");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::endRecordingRHI(const GfRenderContext& kCtx)
{
	VkResult eResult = vkEndCommandBuffer(m_cmdBuffer);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to end command buffer");
}

void GfCmdBuffer_Platform::beginRenderPass(const GfRenderContext& ctx, GfRenderPass* renderPass)
{
	renderPass->Plat().getOrCreateRenderPass(ctx);
	VkRenderPass renderPassVK = renderPass->Plat().getRenderPass();
	VkFramebuffer frameBufferVK = renderPass->Plat().getFramebuffer();

	bool usesDepth = renderPass->usesDepthAttachment();
	u32 attachmentCount = renderPass->getAttachmentCount() + (usesDepth ? 1 : 0);

	StackMemBlock clearColorsMem(static_cast<u32>(sizeof(VkClearValue)) * attachmentCount);
	VkClearValue* clearColors = reinterpret_cast<VkClearValue*>(clearColorsMem.get());
	u32 idx = 0;
	if (usesDepth) 
	{
		VkClearValue& depthClear = clearColors[idx++];
		depthClear.depthStencil.depth = renderPass->getDepthClear();
		depthClear.depthStencil.stencil = renderPass->getStencilClear();
	}

	v4 clearColor = renderPass->getClearColor();
	for (; idx < attachmentCount; ++idx) 
	{
		VkClearValue& clear = clearColors[idx];
		clear.color.float32[0] = clearColor[0];
		clear.color.float32[1] = clearColor[1];
		clear.color.float32[2] = clearColor[2];
		clear.color.float32[3] = clearColor[3];
	}

	VkRenderPassBeginInfo passBeginInfo{};
	passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	passBeginInfo.pNext = nullptr;
	passBeginInfo.renderPass = renderPassVK;
	passBeginInfo.framebuffer = frameBufferVK;
	passBeginInfo.renderArea.extent.width = renderPass->getWidth();
	passBeginInfo.renderArea.extent.height = renderPass->getHeight();
	passBeginInfo.renderArea.offset.x = renderPass->getOffsetX();
	passBeginInfo.renderArea.offset.y = renderPass->getOffsetY();
	passBeginInfo.clearValueCount = attachmentCount;
	passBeginInfo.pClearValues = clearColors;
	vkCmdBeginRenderPass(getCmdBuffer(), &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void GfCmdBuffer_Platform::endRenderPass()
{
	vkCmdEndRenderPass(getCmdBuffer());
}

void GfCmdBuffer_Platform::bindShaderPipe(GfShaderPipeline* pipeline, u32 variantHash,
		const GfShaderPipeConfig* config, u64 configHash,
		const GfVertexDeclaration* vertexFormat,
		const GfRenderPass* renderPass)
{
	GF_ASSERT_ALWAYS("TODO");
}

void GfCmdBuffer_Platform::setViewport(const GfRenderContext& ctx, const GfViewport& viewport)
{
	VkViewport viewportVK;
	viewportVK.x = viewport.m_fOffsetX;
	viewportVK.y = viewport.m_fOffsetY;
	viewportVK.minDepth = viewport.m_fMinDepth;
	viewportVK.maxDepth = viewport.m_fMaxDepth;
	viewportVK.width = viewport.m_fWidth;
	viewportVK.height = viewport.m_fHeight;
	vkCmdSetViewport(getCmdBuffer(), 0, 1, &viewportVK);
}

void GfCmdBuffer_Platform::setScissors(const GfRenderContext& ctx, const GfScissor& scissor)
{
	VkRect2D scissorVK;
	scissorVK.offset.x = scissor.m_siOffsetX;
	scissorVK.offset.y = scissor.m_siOffsetY;
	scissorVK.extent.width = scissor.m_siWidth;
	scissorVK.extent.height = scissor.m_siHeight;
	vkCmdSetScissor(getCmdBuffer(), 0, 1, &scissorVK);
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::drawIndexedRHI(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset /*= 0*/, u32 uiVertexOffset /*= 0*/, u32 uiFirstInstanceId /*= 0*/)
{
	vkCmdDrawIndexed(m_cmdBuffer, uiIdxCount, uiInstanceCount, uiIdxOffset, uiVertexOffset, uiFirstInstanceId);
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::drawRHI(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex /*= 0*/, u32 uiFirstInstance /*= 0*/)
{
	vkCmdDraw(m_cmdBuffer, uiVertexCount, uiInstanceCount, uiFirstVertex, uiFirstInstance);
}

void GfCmdBuffer_Platform::bindVertexBuffersRHI(GfBuffer** vertexBuffers, u32* vertexBufferOffsets, u32 vertexBufferCount)
{
	GfFrameMTStackAlloc::GfMemScope kMemScope(GfFrameMTStackAlloc::Get());

	VkDeviceSize* offsets(GfFrameMTStackAlloc::Get()->Alloc<VkDeviceSize>(vertexBufferCount));
	VkBuffer* buffers(GfFrameMTStackAlloc::Get()->Alloc<VkBuffer>(vertexBufferCount));
	for (u32 i = 0; i < vertexBufferCount; ++i) 
	{
		offsets[i] = static_cast<VkDeviceSize>(vertexBufferOffsets[i]);
		buffers[i] = vertexBuffers[i]->Plat().GetHandle();
	}
	vkCmdBindVertexBuffers(m_cmdBuffer, 0, vertexBufferCount, buffers, offsets);
}

void GfCmdBuffer_Platform::bindIndexBufferRHI(const GfBuffer& buffer, u32 offset, bool useShort)
{
	vkCmdBindIndexBuffer(m_cmdBuffer, 
		buffer.Plat().GetHandle(), 
		static_cast<VkDeviceSize>(offset), 
		useShort ? VkIndexType::VK_INDEX_TYPE_UINT16 : VkIndexType::VK_INDEX_TYPE_UINT32);
}

////////////////////////////////////////////////////////////////////////////////
// EOF