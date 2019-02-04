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
#include "GfRender/Common/GfBuffer.h"
#include "GfRender/Common/GfMaterial.h"
#include "GfRender/Common/GfMatParamLayout.h"

////////////////////////////////////////////////////////////////////////////////

#define GF_INFINITE_TIMEOUT 1000000000

////////////////////////////////////////////////////////////////////////////////

GfCmdBuffer_Platform::GfCmdBuffer_Platform()
	: m_pCmdBuffer(nullptr)
	, m_pFence(0)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::InitRHI (VkCommandBuffer pCmdBuffer, VkFence pFence)
{
	GF_ASSERT(!pCmdBuffer && !m_pFence, "CMDBuffer already initialised");
	m_pCmdBuffer = pCmdBuffer;
	m_pFence = pFence;
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::WaitForReadyRHI(const GfRenderContext& kCtx)
{
	VkResult eResult = vkWaitForFences(kCtx.m_pDevice, 1, &m_pFence, VK_FALSE, GF_INFINITE_TIMEOUT);
	GF_ASSERT(eResult == VK_SUCCESS, "Waited to long for fences");
	eResult = vkResetFences(kCtx.m_pDevice, 1, &m_pFence);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to reset the fences");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::SubmitRHI(
	const GfRenderContext& kCtx,
	GfRencerContextFamilies::Type eQueueType,
	Bool bLast)
{
	const GfFrameSyncing& kSyncPrimitives(kCtx.GetFrameSyncPrimitives());
	VkPipelineStageFlags uiFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo kInfo{};
	kInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	kInfo.pNext = nullptr;
	if (true || bLast) 
	{
		kInfo.waitSemaphoreCount = 1;
		kInfo.pWaitSemaphores = &kSyncPrimitives.m_pImageReady;
		kInfo.pWaitDstStageMask = &uiFlags;
		kInfo.signalSemaphoreCount = 1;
		kInfo.pSignalSemaphores = &kSyncPrimitives.m_pFinishedRendering;
	}
	kInfo.commandBufferCount = 1;
	kInfo.pCommandBuffers = &m_pCmdBuffer;

	VkResult eResult = vkQueueSubmit(kCtx.GetQueue(eQueueType), 1, &kInfo, m_pFence);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to submit cmd block");
}

////////////////////////////////////////////////////////////////////////////////
// Render pass commands

void GfCmdBuffer_Platform::BeginRenderPassRHI(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass)
{
	GfWindow* pWindow(kCtx.GetWindow());
	// Begin render pass

	// TODO: Refactor this
	VkClearValue clearColor = { 1.0f, 0.0f, 0.0f, 0.0f };

	VkRenderPassBeginInfo passBeginInfo{};
	passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	passBeginInfo.pNext = nullptr;
	passBeginInfo.renderPass = kRenderPass.m_pRenderPass;
	passBeginInfo.framebuffer = kRenderPass.m_pFramebuffers[kCtx.GetCurrentFrameIdx()];
	passBeginInfo.renderArea.extent.width = pWindow->GetWidth();
	passBeginInfo.renderArea.extent.height = pWindow->GetHeight();
	passBeginInfo.renderArea.offset.x = 0;
	passBeginInfo.renderArea.offset.y = 0;
	passBeginInfo.clearValueCount = 1;
	passBeginInfo.pClearValues = &clearColor;
	vkCmdBeginRenderPass(m_pCmdBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::EndRenderPassRHI(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass)
{
	vkCmdEndRenderPass(m_pCmdBuffer);
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::BeginRecordingRHI(const GfRenderContext& kCtx)
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
	VkResult eResult = vkBeginCommandBuffer(m_pCmdBuffer, &kInfo);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to begin cmd buffer");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::EndRecordingRHI(const GfRenderContext& kCtx)
{
	VkResult eResult = vkEndCommandBuffer(m_pCmdBuffer);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to end command buffer");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::ClearCurrentTargetRHI(const GfRenderContext& kCtx, const v4& vClearColor)
{
	// TODO: Refactor this to get rid of the barriers. This is just a temporal fix and I'll use the RenderPasses in the future

	VkImageSubresourceRange kImageRange = {};
	kImageRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	kImageRange.levelCount = 1;
	kImageRange.layerCount = 1;

	////////////////////////////////////////////////////////////////////////////////

	VkImageMemoryBarrier kBarrierPresentToClear = 
	{
		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,     // VkStructureType                        sType
		nullptr,                                    // const void                            *pNext
		VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags                          srcAccessMask
		VK_ACCESS_TRANSFER_WRITE_BIT,               // VkAccessFlags                          dstAccessMask
		VK_IMAGE_LAYOUT_UNDEFINED,                  // VkImageLayout                          oldLayout
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,       // VkImageLayout                          newLayout
		kCtx.GetFamilyIdx(GfRencerContextFamilies::Present),             // uint32_t                               srcQueueFamilyIndex
		kCtx.GetFamilyIdx(GfRencerContextFamilies::Present),             // uint32_t                               dstQueueFamilyIndex
		kCtx.GetCurrentBackBuffer(),                       // VkImage                                image
		kImageRange                     // VkImageSubresourceRange                subresourceRange
	};

	VkImageMemoryBarrier kBarrierClearToPresent = 
	{
		VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,     // VkStructureType                        sType
		nullptr,                                    // const void                            *pNext
		VK_ACCESS_TRANSFER_WRITE_BIT,               // VkAccessFlags                          srcAccessMask
		VK_ACCESS_MEMORY_READ_BIT,                  // VkAccessFlags                          dstAccessMask
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,       // VkImageLayout                          oldLayout
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,            // VkImageLayout                          newLayout
		kCtx.GetFamilyIdx(GfRencerContextFamilies::Present),             // uint32_t                               srcQueueFamilyIndex
		kCtx.GetFamilyIdx(GfRencerContextFamilies::Present),             // uint32_t                               dstQueueFamilyIndex
		kCtx.GetCurrentBackBuffer(),                       // VkImage                                image
		kImageRange                     // VkImageSubresourceRange                subresourceRange
	};


	////////////////////////////////////////////////////////////////////////////////

		vkCmdPipelineBarrier(
			m_pCmdBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, 
			VK_PIPELINE_STAGE_TRANSFER_BIT, 
			0, 0, nullptr, 0, nullptr, 1, &kBarrierPresentToClear);

	////////////////////////////////////////////////////////////////////////////////

	VkClearColorValue clearColor{ vClearColor.x, vClearColor.y, vClearColor.z, vClearColor.w };
	VkClearValue clearValue = {};
	clearValue.color = clearColor;

	vkCmdClearColorImage(m_pCmdBuffer, kCtx.GetCurrentBackBuffer(), VK_IMAGE_LAYOUT_GENERAL, &clearColor, 1, &kImageRange);

	////////////////////////////////////////////////////////////////////////////////

	vkCmdPipelineBarrier(m_pCmdBuffer, 
		VK_PIPELINE_STAGE_TRANSFER_BIT, 
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &kBarrierClearToPresent);

}

////////////////////////////////////////////////////////////////////////////////
// Buffer commands

void GfCmdBuffer_Platform::CopyBufferRangeRHI(
	const GfRenderContext& kCtx, 
	const GfBuffer& kFrom, 
	const GfBuffer& kTo, 
	u32 uiFromOffset, u32 uiToOffset, u32 uiSize)
{
	GfStageAccessConfig kCurrToTransferSettings = kTo.GetTransitionSettings();
	GfStageAccessConfig kTransferToCurrSettings = GfBuffer_Platform::GetTransitionSettingsForType(EBufferUsage::Transfer_Dst);

	VkBufferMemoryBarrier kBarrier;
	kBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	kBarrier.pNext = nullptr;
	kBarrier.buffer = kTo.GetHandle();
	kBarrier.offset = uiToOffset;
	kBarrier.size = uiSize;
	kBarrier.srcAccessMask = kCurrToTransferSettings.m_eAccess;
	kBarrier.dstAccessMask = kTransferToCurrSettings.m_eAccess;
	// TODO: Do I need to transit between different queues
	kBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	kBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	vkCmdPipelineBarrier(
		m_pCmdBuffer,
		kCurrToTransferSettings.m_eStage,
		kTransferToCurrSettings.m_eStage,
		0,
		0, nullptr,
		1, &kBarrier,
		0, nullptr);

	VkBufferCopy kRegion;
	kRegion.srcOffset = uiFromOffset;
	kRegion.dstOffset = uiToOffset;
	kRegion.size = uiSize;
	vkCmdCopyBuffer(m_pCmdBuffer, kFrom.GetHandle(), kTo.GetHandle(), 1, &kRegion);

	// Go back to the original state
	kBarrier.srcAccessMask = kTransferToCurrSettings.m_eAccess;
	kBarrier.dstAccessMask = kCurrToTransferSettings.m_eAccess;
	vkCmdPipelineBarrier(
		m_pCmdBuffer,
		kTransferToCurrSettings.m_eStage,
		kCurrToTransferSettings.m_eStage,
		0,
		0, nullptr,
		1, &kBarrier,
		0, nullptr);
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::UpdateBufferRangeRHI(
	const GfRenderContext& kCtx, 
	const GfBuffer& kBuffer, 
	u32 uiOffset, u32 uiSize, void* pData)
{
	GfStageAccessConfig kCurrToTransferSettings = kBuffer.GetTransitionSettings();
	GfStageAccessConfig kTransferToCurrSettings = GfBuffer_Platform::GetTransitionSettingsForType(EBufferUsage::Transfer_Dst);

	// Sync to transfer operations
	VkBufferMemoryBarrier barrier;
	barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	barrier.pNext = nullptr;
	barrier.buffer = kBuffer.GetHandle();
	barrier.offset = uiOffset;
	barrier.size = uiSize;
	barrier.srcAccessMask = kCurrToTransferSettings.m_eAccess;
	barrier.dstAccessMask = kTransferToCurrSettings.m_eAccess;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	vkCmdPipelineBarrier(
		m_pCmdBuffer,
		kCurrToTransferSettings.m_eStage,
		kTransferToCurrSettings.m_eStage,
		0,
		0, nullptr,
		1, &barrier,
		0, nullptr);

	// Update data
	vkCmdUpdateBuffer(m_pCmdBuffer, kBuffer.GetHandle(), uiOffset, uiSize, pData);

	// Sync back
	barrier.srcAccessMask = kTransferToCurrSettings.m_eAccess;
	barrier.dstAccessMask = kCurrToTransferSettings.m_eAccess;
	vkCmdPipelineBarrier(
		m_pCmdBuffer,
		kTransferToCurrSettings.m_eStage,
		kCurrToTransferSettings.m_eStage,
		0,
		0, nullptr,
		1, &barrier,
		0, nullptr);
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::BindParameterSetRHI(
	const GfMaterialTemplate& kMaterial, const GfMaterialParamSet& kparamSet, 
	u32 uiBindPoint, bool bIsGraphics)
{
	VkPipelineBindPoint uiBindType(bIsGraphics ? VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE);
	VkDescriptorSet pDescriptorSet(kparamSet.GetDescriptorSet());
	vkCmdBindDescriptorSets(m_pCmdBuffer, uiBindType,
		kMaterial.GetLayout(), uiBindPoint, 1, &pDescriptorSet, 0, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// EOF