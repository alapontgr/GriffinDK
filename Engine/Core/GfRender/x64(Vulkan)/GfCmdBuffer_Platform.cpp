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

#include "GfRender/Common/GfCmdBuffer.h"
#include "GfRender/Common/GfRenderContext.h"
#include "GfRender/Common/GfRenderPass.h"
#include "GfRender/Common/GfWindow.h"

////////////////////////////////////////////////////////////////////////////////

#define GF_INFINITE_TIMEOUT 1000000000

////////////////////////////////////////////////////////////////////////////////

GfCmdBuffer_Platform::GfCmdBuffer_Platform()
{
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::InitPlatform (
	VkCommandBuffer* pCmdBuffers,
	VkFence* pFences)
{
	for (u32 i=0; i<GfRenderConstants::ms_uiNBufferingCount; ++i)
	{
		m_pEntries[i].m_pCmdBuffer = pCmdBuffers[i];
		m_pEntries[i].m_pFence = pFences[i];
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::WaitForReadyPlatform(const GfRenderContext& kCtx)
{
	GfCmdBufferSlot_Platform& kCurrEntry(m_pEntries[kCtx.GetCurrentFrameIdx()]);
	VkResult eResult = vkWaitForFences(kCtx.m_pDevice, 1, &kCurrEntry.m_pFence, VK_FALSE, GF_INFINITE_TIMEOUT);
	GF_ASSERT(eResult == VK_SUCCESS, "Waited to long for fences");
	eResult = vkResetFences(kCtx.m_pDevice, 1, &kCurrEntry.m_pFence);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to reset the fences");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::SubmitPlatform(
	const GfRenderContext& kCtx,
	GfRencerContextFamilies::Type eQueueType,
	Bool bLast)
{
	GfCmdBufferSlot_Platform& kCurrEntry(m_pEntries[kCtx.GetCurrentFrameIdx()]);
	const GfFrameSyncing& kSyncPrimitives(kCtx.GetFrameSyncPrimitives());
	VkPipelineStageFlags uiFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo kInfo{};
	kInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	kInfo.pNext = nullptr;
	kInfo.waitSemaphoreCount = 1;
	if (bLast) 
	{
		kInfo.pWaitSemaphores = &kSyncPrimitives.m_pImageReady;
		kInfo.pWaitDstStageMask = &uiFlags;
		kInfo.pSignalSemaphores = &kSyncPrimitives.m_pFinishedRendering;
	}
	kInfo.commandBufferCount = 1;
	kInfo.signalSemaphoreCount = 1;
	kInfo.pCommandBuffers = &kCurrEntry.m_pCmdBuffer;

	VkResult eResult = vkQueueSubmit(kCtx.GetQueue(eQueueType), 1, &kInfo,
		kCurrEntry.m_pFence);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to submit cmd block");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::BeginRenderPassPlatform(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass)
{
	GfCmdBufferSlot_Platform& kCurrEntry(m_pEntries[kCtx.GetCurrentFrameIdx()]);
	GfWindow* pWindow(kCtx.GetWindow());
	// Begin render pass

	// TODO: Refactor this
	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };

	VkRenderPassBeginInfo passBeginInfo{};
	passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	passBeginInfo.pNext = nullptr;
	passBeginInfo.renderPass = kRenderPass.m_pRenderPass;
	passBeginInfo.framebuffer = kRenderPass.m_pFramebuffer;
	passBeginInfo.renderArea.extent.width = pWindow->GetWidth();
	passBeginInfo.renderArea.extent.height = pWindow->GetHeight();
	passBeginInfo.renderArea.offset.x = 0;
	passBeginInfo.renderArea.offset.y = 0;
	passBeginInfo.clearValueCount = 1;
	passBeginInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(kCurrEntry.m_pCmdBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::EndRenderPassPlatform(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass)
{
	GfCmdBufferSlot_Platform& kCurrEntry(m_pEntries[kCtx.GetCurrentFrameIdx()]);
	vkCmdEndRenderPass(kCurrEntry.m_pCmdBuffer);
}

////////////////////////////////////////////////////////////////////////////////
// EOF