////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/16
//	File: 	GfCmdBuffer.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFCMDBUFFER_H__
#define __GFCMDBUFFER_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfCore/GfCoreMinimal.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GfCmdBuffer_Platform.h)

#include "Common/GfRender/GfShaderPipeline.h"

////////////////////////////////////////////////////////////////////////////////

namespace GfCmdBufferType
{
	enum Type : u32
	{
		Invalid,
		Primary,
		Secondary
	};
}

////////////////////////////////////////////////////////////////////////////////

struct GfRenderPipelineState 
{
	const GfVertexDeclaration* m_curVertexFormat = nullptr;
	GfRenderPass* m_curRenderPass = nullptr;
	GfShaderPipeline* m_curPipeline = nullptr;
	GfVariantHash m_curVariantHash = 0;
	GfShaderPipeConfig m_config;
};

class GfBuffer;

class GfCmdBuffer
{
	GF_DECLARE_PLATFORM_INTERFACE(GfCmdBuffer);
public:

	friend class GfCmdBuffer_Platform;
	friend class GfCmdBufferCache;

	static GfCmdBuffer* get(const GfRenderContext* ctx, 
		GfCmdBufferType::Type type,
		GfRenderContextFamilies::Type queue);

	////////////////////////////////////////////////////////////////////////////////
	// CmdBuffer commands

	void beginRecording();

	void endRecording();

	bool isReady();

	// Sync point to avoid start recording while the command buffer is still being processed
	void waitForReady();

	void submit(const GfWindow& kWindow, Bool bLast);

	////////////////////////////////////////////////////////////////////////////////

	void setBlendState(const GfBlendState& blendState);

	void setRasterState(const GfRasterState& rasterState);

	void setDepthState(const GfDepthState& depthState);

	void setTopology(EPrimitiveTopology::Type topology);

	void setMSAAState(const GfMultiSamplingState& msaaState);

	void setVertexFormat(const GfVertexDeclaration* vertexFormat);

	////////////////////////////////////////////////////////////////////////////////

	void beginRenderPass(GfRenderPass* renderPass);
	
	void endRenderPass();

	void bindShaderPipe(const class GfShaderVariant& shaderVariant);

	void bindVertexBuffers(GfBuffer** vertexBuffers, u32* vertexBufferOffsets, u32 vertexBufferCount);
	
	void bindIndexBuffer(const GfBuffer& buffer, u32 offset, bool useShort);

	void drawIndexed(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset = 0, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

	void draw(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

private:

	GfCmdBuffer();

	GfCmdBuffer(const GfRenderContext* ctx, GfCmdBufferCache* cache,
		const GfCmdBufferType::Type type, GfRenderContextFamilies::Type queue);

	void reset();

	void drawcallCommon();

	GfCmdBufferType::Type	m_type;
	GfRenderContextFamilies::Type m_queue;
	const GfRenderContext* m_ctx;
	class GfCmdBufferCache* m_cache;

	GfRenderPipelineState m_curState;
};

////////////////////////////////////////////////////////////////////////////////

class GfCmdBufferFactory
{
	GF_DECLARE_PLATFORM_INTERFACE(GfCmdBufferFactory);
public:

	GfCmdBufferFactory();

	void init(const GfRenderContext& kCtx, GfRenderContextFamilies::Type eQueueType);

	void shutdown(const GfRenderContext& ctx);

	bool isInitialized() const { return m_initialized; }

private:

	bool m_initialized;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::waitForReady()
{
	m_kPlatform.waitForReadyRHI(*m_ctx);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::beginRecording()
{
	m_kPlatform.beginRecordingRHI(*m_ctx);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::endRecording()
{
	m_kPlatform.endRecordingRHI(*m_ctx);
}

GF_FORCEINLINE bool GfCmdBuffer::isReady()
{
	return m_kPlatform.isReady(*m_ctx);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFER_H__