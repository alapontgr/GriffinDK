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

	GfCmdBuffer();

	void init(GfCmdBufferType::Type eType);

	////////////////////////////////////////////////////////////////////////////////
	// CmdBuffer commands

	void beginRecording(const GfRenderContext& kCtx);

	void endRecording(const GfRenderContext& kCtx);

	// Sync point to avoid start recording while the command buffer is still being processed
	void waitForReady(const GfRenderContext& kCtx);

	void submit(
		const GfRenderContext& kCtx,
		const GfWindow& kWindow,
		GfRenderContextFamilies::Type eQueueType,
		Bool bLast);

	////////////////////////////////////////////////////////////////////////////////

	void drawIndexed(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset = 0, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

	void draw(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

	void bindVertexBuffers(GfBuffer** vertexBuffers, u32* vertexBufferOffsets, u32 vertexBufferCount);
	
	void bindIndexBuffer(const GfBuffer& buffer, u32 offset, bool useShort);

	////////////////////////////////////////////////////////////////////////////////

private:

	GfCmdBufferType::Type	m_eType;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::waitForReady(const GfRenderContext& kCtx)
{
	m_kPlatform.waitForReadyRHI(kCtx);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::submit(
	const GfRenderContext& kCtx,
	const GfWindow& kWindow,
	GfRenderContextFamilies::Type eQueueType,
	Bool bLast)
{
	m_kPlatform.submitRHI(kCtx, kWindow, eQueueType, bLast);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::beginRecording(const GfRenderContext& kCtx)
{
	m_kPlatform.beginRecordingRHI(kCtx);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::endRecording(const GfRenderContext& kCtx)
{
	m_kPlatform.endRecordingRHI(kCtx);
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