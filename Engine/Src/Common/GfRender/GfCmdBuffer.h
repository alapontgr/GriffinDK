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

	void beginRenderPass(GfRenderPass* renderPass);
	
	void endRenderPass();

	void bindShaderPipe(const class GfShaderVariant& shaderVariant);

	////////////////////////////////////////////////////////////////////////////////

	void drawIndexed(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset = 0, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

	void draw(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

	void bindVertexBuffers(GfBuffer** vertexBuffers, u32* vertexBufferOffsets, u32 vertexBufferCount);
	
	void bindIndexBuffer(const GfBuffer& buffer, u32 offset, bool useShort);

	////////////////////////////////////////////////////////////////////////////////

private:

	GfCmdBuffer();

	GfCmdBuffer(const GfRenderContext* ctx, GfCmdBufferCache* cache,
		const GfCmdBufferType::Type type, GfRenderContextFamilies::Type queue);

	GfCmdBufferType::Type	m_type;
	GfRenderContextFamilies::Type m_queue;
	const GfRenderContext* m_ctx;
	class GfCmdBufferCache* m_cache;
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

GF_FORCEINLINE void GfCmdBuffer::drawIndexed(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset /*= 0*/, u32 uiVertexOffset /*= 0*/, u32 uiFirstInstanceId /*= 0*/)
{
	m_kPlatform.drawIndexedRHI(uiIdxCount, uiInstanceCount, uiIdxOffset, uiVertexOffset, uiFirstInstanceId);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::draw(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex /*= 0*/, u32 uiFirstInstance /*= 0*/)
{
	m_kPlatform.drawRHI(uiVertexCount, uiInstanceCount, uiFirstVertex, uiFirstInstance);
}

GF_FORCEINLINE void GfCmdBuffer::bindVertexBuffers(GfBuffer** vertexBuffers, u32* vertexBufferOffsets, u32 vertexBufferCount) 
{
	m_kPlatform.bindVertexBuffersRHI(vertexBuffers, vertexBufferOffsets, vertexBufferCount);
}

GF_FORCEINLINE void GfCmdBuffer::bindIndexBuffer(const GfBuffer& buffer, u32 offset, bool useShort)
{
	m_kPlatform.bindIndexBufferRHI(buffer, offset, useShort);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFER_H__