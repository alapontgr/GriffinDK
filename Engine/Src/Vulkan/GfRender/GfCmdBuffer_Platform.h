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

	VkCommandBuffer getCmdBuffer() const { return m_cmdBuffer; }

	VkFence getFence() const { return m_fence; }

private:

	void initRHI(const GfRenderContext& ctx, const class GfCmdBufferFactory& factory, u32 type);

	void waitForReadyRHI(const GfRenderContext& ctx);

	bool isReady(const GfRenderContext& ctx);

	void submitRHI(
		const GfRenderContext& kCtx, 
		const GfWindow& kWindow,
		GfRenderContextFamilies::Type eQueueType, 
		Bool bLast);

	////////////////////////////////////////////////////////////////////////////////

	void beginRecordingRHI(const GfRenderContext& kCtx);

	void endRecordingRHI(const GfRenderContext& kCtx);

	////////////////////////////////////////////////////////////////////////////////

	void beginRenderPass(const GfRenderContext& kCtx, GfRenderPass* renderPass);
	
	void endRenderPass();

	void bindShaderPipe(class GfShaderPipeline* pipeline, u32 variantHash,
		const struct GfShaderPipeConfig* config,
		const class GfVertexDeclaration* vertexFormat,
		const class GfRenderPass* renderPass);

	void setViewport(const GfRenderContext& ctx, const GfViewport& viewport);

	void setScissors(const GfRenderContext& ctx, const GfScissor& scissor);

	////////////////////////////////////////////////////////////////////////////////

	void drawIndexedRHI(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset = 0, u32 uiVertexOffset = 0, u32 uiFirstInstanceId = 0);

	void drawRHI(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

	void bindVertexBuffersRHI(GfBuffer** vertexBuffers, u32* vertexBufferOffsets, u32 vertexBufferCount);

	void bindIndexBufferRHI(const GfBuffer& buffer, u32 offset, bool useShort);

	////////////////////////////////////////////////////////////////////////////////

	// Perform Multi buffering of the command buffers to avoid waiting for the end of a previous execution
	VkCommandBuffer m_cmdBuffer;
	VkFence			m_fence;
};

class GfCmdBufferFactory_Platform
{
public:

	friend class GfCmdBufferFactory;

	GfCmdBufferFactory_Platform();

	VkCommandPool getPool() const { return m_pool; }

private:

	void initRHI(const GfRenderContext& ctx, GfRenderContextFamilies::Type queueType);

	void shutdown(const GfRenderContext& ctx);

	VkCommandPool m_pool;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFER_PLATFORM_H__