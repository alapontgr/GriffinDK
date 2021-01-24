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

#include "Common/GfCore/GfMaths.h"
#include "Common/GfRender/GfGraphicsSDK.h"
#include "Common/GfRender/GfRenderConstants.h"

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

	void initRHI(VkCommandBuffer pCmdBuffer, VkFence pFence);

	void waitForReadyRHI(const GfRenderContext& kCtx);

	void submitRHI(
		const GfRenderContext& kCtx, 
		const GfWindow& kWindow,
		GfRenderContextFamilies::Type eQueueType, 
		Bool bLast);

	////////////////////////////////////////////////////////////////////////////////
	// Render pass commands

	void beginRecordingRHI(const GfRenderContext& kCtx);

	void endRecordingRHI(const GfRenderContext& kCtx);

	////////////////////////////////////////////////////////////////////////////////

	void drawIndexedRHI(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset = 0, u32 uiVertexOffset = 0, u32 uiFirstInstanceId = 0);

	void drawRHI(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

	void bindVertexBuffersRHI(GfBuffer** vertexBuffers, u32* vertexBufferOffsets, u32 vertexBufferCount);

	void bindIndexBufferRHI(const GfBuffer& buffer, u32 offset, bool useShort);

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