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
#include "Common/GfRender/GfMaterial.h"
#include "Common/GfRender/GfMatParamLayout.h"

////////////////////////////////////////////////////////////////////////////////

#define GF_INFINITE_TIMEOUT 1000000000

////////////////////////////////////////////////////////////////////////////////

GfCmdBuffer_Platform::GfCmdBuffer_Platform()
	: m_pCmdBuffer(nullptr)
	, m_pFence(0)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::initRHI (VkCommandBuffer pCmdBuffer, VkFence pFence)
{
	GF_ASSERT(!m_pCmdBuffer && !m_pFence, "CMDBuffer already initialised");
	m_pCmdBuffer = pCmdBuffer;
	m_pFence = pFence;
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::waitForReadyRHI(const GfRenderContext& kCtx)
{
	VkResult eResult = vkWaitForFences(kCtx.Plat().m_pDevice, 1, &m_pFence, VK_FALSE, GF_INFINITE_TIMEOUT);
	GF_ASSERT(eResult == VK_SUCCESS, "Waited to long for fences");
	eResult = vkResetFences(kCtx.Plat().m_pDevice, 1, &m_pFence);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to reset the fences");
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
	kInfo.pCommandBuffers = &m_pCmdBuffer;

	VkResult eResult = vkQueueSubmit(kCtx.Plat().GetQueue(eQueueType), 1, &kInfo, m_pFence);
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
	VkResult eResult = vkBeginCommandBuffer(m_pCmdBuffer, &kInfo);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to begin cmd buffer");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::endRecordingRHI(const GfRenderContext& kCtx)
{
	VkResult eResult = vkEndCommandBuffer(m_pCmdBuffer);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to end command buffer");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::drawIndexedRHI(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset /*= 0*/, u32 uiVertexOffset /*= 0*/, u32 uiFirstInstanceId /*= 0*/)
{
	vkCmdDrawIndexed(m_pCmdBuffer, uiIdxCount, uiInstanceCount, uiIdxOffset, uiVertexOffset, uiFirstInstanceId);
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::drawRHI(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex /*= 0*/, u32 uiFirstInstance /*= 0*/)
{
	vkCmdDraw(m_pCmdBuffer, uiVertexCount, uiInstanceCount, uiFirstVertex, uiFirstInstance);
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
	vkCmdBindVertexBuffers(m_pCmdBuffer, 0, vertexBufferCount, buffers, offsets);
}

void GfCmdBuffer_Platform::bindIndexBufferRHI(const GfBuffer& buffer, u32 offset, bool useShort)
{
	vkCmdBindIndexBuffer(m_pCmdBuffer, 
		buffer.Plat().GetHandle(), 
		static_cast<VkDeviceSize>(offset), 
		useShort ? VkIndexType::VK_INDEX_TYPE_UINT16 : VkIndexType::VK_INDEX_TYPE_UINT32);
}

////////////////////////////////////////////////////////////////////////////////
// EOF