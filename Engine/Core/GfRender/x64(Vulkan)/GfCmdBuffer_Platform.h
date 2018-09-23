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

	void InitPlatform(
		VkCommandBuffer* pCmdBuffers,
		VkFence* pFences);

	void WaitForReadyPlatform(const GfRenderContext& kCtx);

	void SubmitPlatform(
		const GfRenderContext& kCtx, 
		GfRencerContextFamilies::Type eQueueType, 
		Bool bLast);

	////////////////////////////////////////////////////////////////////////////////

	void BeginRenderPassPlatform(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass);

	void EndRenderPassPlatform(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass);

	void BeginRecordingPlatform(const GfRenderContext& kCtx);

	void EndRecordingPlatform(const GfRenderContext& kCtx);

	void ClearCurrentTargetPlatform(const GfRenderContext& kCtx, const v4& vClearColor);

	// Perform Multi buffering of the command buffers to avoid waiting for the end of a previous execution
	GfCmdBufferSlot_Platform m_pEntries[GfRenderConstants::ms_uiNBufferingCount];
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFER_PLATFORM_H__