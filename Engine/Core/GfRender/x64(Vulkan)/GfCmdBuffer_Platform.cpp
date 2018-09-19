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
	// TODO: Continue here
#ifdef DEAD
	GfCmdBufferSlot_Platform& kCurrEntry(m_pEntries[kCtx.GetCurrentFrameIdx()]);
	const GfFrameSyncing& kSyncPrimitives(kCtx.GetFrameSyncPrimitives());
	VkPipelineStageFlags uiFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo kInfo{};
	kInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	kInfo.pNext = nullptr;
	kInfo.waitSemaphoreCount = 1;
	kInfo.pWaitSemaphores = &frame.imageAvailable;
	kInfo.pWaitDstStageMask = &uiFlags;
	kInfo.commandBufferCount = 1;
	kInfo.signalSemaphoreCount = 1;
	kInfo.pSignalSemaphores = &frame.finishedRendering;
	kInfo.pCommandBuffers = &kCurrEntry.m_pCmdBuffer;

	VkResult eResult = vkQueueSubmit(m_presentQueue, 1, &kInfo,
		frame.cmdBufferReady);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to submit cmd block");
#endif // DEAD
}

////////////////////////////////////////////////////////////////////////////////
// EOF