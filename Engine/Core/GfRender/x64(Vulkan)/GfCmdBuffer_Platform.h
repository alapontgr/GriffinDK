////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/16
//	File: 	GfCmdBuffer_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFCMDBUFFER_PLATFORM_H__
#define __GFCMDBUFFER_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfCore/Common/GfMaths.h"
#include "GfRender/Common/GfGraphicsSDK.h"
#include "GfRender/Common/GfRenderConstants.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;
class GfRenderPass;
class GfBuffer;

////////////////////////////////////////////////////////////////////////////////

struct GfCmdBufferSlot_Platform
{
	VkCommandBuffer m_pCmdBuffer	= nullptr;
	VkFence			m_pFence		= 0;
};

////////////////////////////////////////////////////////////////////////////////

class GfCmdBuffer_Platform
{
public:

	friend class GfCmdBufferFactory_Platform;
	friend class GfCmdBuffer;

	GfCmdBuffer_Platform();

private:

	void InitRHI(
		VkCommandBuffer* pCmdBuffers,
		VkFence* pFences);

	void WaitForReadyRHI(const GfRenderContext& kCtx);

	void SubmitRHI(
		const GfRenderContext& kCtx, 
		GfRencerContextFamilies::Type eQueueType, 
		Bool bLast);

	////////////////////////////////////////////////////////////////////////////////
	// Render pass commands

	void BeginRenderPassRHI(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass);

	void EndRenderPassRHI(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass);

	void BeginRecordingRHI(const GfRenderContext& kCtx);

	void EndRecordingRHI(const GfRenderContext& kCtx);

	void ClearCurrentTargetRHI(const GfRenderContext& kCtx, const v4& vClearColor);

	////////////////////////////////////////////////////////////////////////////////
	// Buffer commands

	void CopyBufferRangeRHI(const GfRenderContext& kCtx, const GfBuffer& kFrom, const GfBuffer& kTo, u32 uiFromOffset, u32 uiToOffset, u32 uiSize);

	void UpdateBufferRangeRHI(const GfRenderContext& kCtx, const GfBuffer& kBuffer, u32 uiOffset, u32 uiSize, void* pData);

	////////////////////////////////////////////////////////////////////////////////

	// Perform Multi buffering of the command buffers to avoid waiting for the end of a previous execution
	GfCmdBufferSlot_Platform m_pEntries[GfRenderConstants::ms_uiNBufferingCount];
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFER_PLATFORM_H__