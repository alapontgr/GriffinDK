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
#include "GfRender/Common/GfWindow.h"
#include "GfRender/Common/GfMaterial.h"
#include "GfRender/Common/GfMatParamLayout.h"
#include "GfRender/Common/GraphicResources/GfBuffer.h"

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
	GF_ASSERT(!m_pCmdBuffer && !m_pFence, "CMDBuffer already initialised");
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
	GfRenderContextFamilies::Type eQueueType,
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

void GfCmdBuffer_Platform::DrawIndexedRHI(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset /*= 0*/, u32 uiVertexOffset /*= 0*/, u32 uiFirstInstanceId /*= 0*/)
{
	vkCmdDrawIndexed(m_pCmdBuffer, uiIdxCount, uiInstanceCount, uiIdxOffset, uiVertexOffset, uiFirstInstanceId);
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::DrawRHI(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex /*= 0*/, u32 uiFirstInstance /*= 0*/)
{
	vkCmdDraw(m_pCmdBuffer, uiVertexCount, uiInstanceCount, uiFirstVertex, uiFirstInstance);
}

////////////////////////////////////////////////////////////////////////////////
// EOF