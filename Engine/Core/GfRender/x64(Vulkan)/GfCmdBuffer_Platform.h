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
class GfWindow;
class GfRenderPass;
class GfBuffer;
class GfMaterialTemplate;
class GfMaterialParamSet;
class GfTexture2D;
struct GfViewport;
struct GfScissor;

////////////////////////////////////////////////////////////////////////////////

class GfCmdBuffer_Platform
{
public:

	friend class GfCmdBufferFactory_Platform;
	friend class GfCmdBuffer;

	GfCmdBuffer_Platform();

	VkCommandBuffer GetCmdBuffer() const;

private:

	void InitRHI(VkCommandBuffer pCmdBuffer, VkFence pFence);

	void WaitForReadyRHI(const GfRenderContext& kCtx);

	void SubmitRHI(
		const GfRenderContext& kCtx, 
		const GfWindow& kWindow,
		GfRenderContextFamilies::Type eQueueType, 
		Bool bLast);

	////////////////////////////////////////////////////////////////////////////////
	// Render pass commands

	void BeginRecordingRHI(const GfRenderContext& kCtx);

	void EndRecordingRHI(const GfRenderContext& kCtx);

	////////////////////////////////////////////////////////////////////////////////
	// Draw cmds

	void DrawIndexedRHI(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset = 0, u32 uiVertexOffset = 0, u32 uiFirstInstanceId = 0);

	void DrawRHI(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

	////////////////////////////////////////////////////////////////////////////////

	// Perform Multi buffering of the command buffers to avoid waiting for the end of a previous execution
	VkCommandBuffer m_pCmdBuffer;
	VkFence			m_pFence;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkCommandBuffer GfCmdBuffer_Platform::GetCmdBuffer() const
{
	return m_pCmdBuffer;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFER_PLATFORM_H__