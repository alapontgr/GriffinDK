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

#include "GfCore/Common/GfCoreMinimal.h"
#include GF_SOLVE_PLATFORM_HEADER(GfCmdBuffer)

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

class GfCmdBuffer : public GfCmdBuffer_Platform
{
public:

	friend class GfCmdBuffer_Platform;

	GfCmdBuffer();

	void Init(GfCmdBufferType::Type eType);

	void BeginRecording(const GfRenderContext& kCtx);

	void EndRecording(const GfRenderContext& kCtx);

	// Sync point to avoid start recording while the command buffer is still being processed
	void WaitForReady(const GfRenderContext& kCtx);

	void Submit(
		const GfRenderContext& kCtx,
		GfRencerContextFamilies::Type eQueueType,
		Bool bLast);

	////////////////////////////////////////////////////////////////////////////////
	// Render pass actions

	void BeginRenderPass(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass);

	void EndRenderPass(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass);

	void ClearCurrentTarget(const GfRenderContext& kCtx, const v4& vClearColor);

	////////////////////////////////////////////////////////////////////////////////
	// Buffer commands

	void CopyBufferRange(const GfRenderContext& kCtx, const GfBuffer& kFrom, const GfBuffer& kTo, u32 uiFromOffset, u32 uiToOffset, u32 uiSize);

	void UpdateBufferRange(const GfRenderContext& kCtx, const GfBuffer& kBuffer, u32 uiOffset, u32 uiSize, void* pData);

	////////////////////////////////////////////////////////////////////////////////

private:

	GfCmdBufferType::Type	m_eType;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::WaitForReady(const GfRenderContext& kCtx)
{
	WaitForReadyRHI(kCtx);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::Submit(
	const GfRenderContext& kCtx,
	GfRencerContextFamilies::Type eQueueType,
	Bool bLast)
{
	SubmitRHI(kCtx, eQueueType, bLast);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::ClearCurrentTarget(const GfRenderContext& kCtx, const v4& vClearColor)
{
	ClearCurrentTargetRHI(kCtx, vClearColor);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::BeginRecording(const GfRenderContext& kCtx)
{
	BeginRecordingRHI(kCtx);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::EndRecording(const GfRenderContext& kCtx)
{
	EndRecordingRHI(kCtx);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::BeginRenderPass(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass)
{
	BeginRenderPassRHI(kCtx, kRenderPass);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::EndRenderPass(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass)
{
	EndRenderPassRHI(kCtx, kRenderPass);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::CopyBufferRange(const GfRenderContext& kCtx, const GfBuffer& kFrom, const GfBuffer& kTo, u32 uiFromOffset, u32 uiToOffset, u32 uiSize)
{
	CopyBufferRangeRHI(kCtx, kFrom, kTo, uiFromOffset, uiToOffset, uiSize);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::UpdateBufferRange(const GfRenderContext& kCtx, const GfBuffer& kBuffer, u32 uiOffset, u32 uiSize, void* pData)
{
	UpdateBufferRangeRHI(kCtx, kBuffer, uiOffset, uiSize, pData);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFER_H__