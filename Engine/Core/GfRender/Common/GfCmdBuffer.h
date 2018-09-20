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

class GfRenderContext;

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

	void StartRecording();

	void StopRecording();

	// Sync point to avoid start recording while the command buffer is still being processed
	void WaitForReady(const GfRenderContext& kCtx);

	void Submit(
		const GfRenderContext& kCtx,
		GfRencerContextFamilies::Type eQueueType,
		Bool bLast);

	////////////////////////////////////////////////////////////////////////////////

	void BeginRenderPass(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass);

	void EndRenderPass(const GfRenderContext& kCtx, const GfRenderPass& kRenderPass);

	////////////////////////////////////////////////////////////////////////////////

private:

	GfCmdBufferType::Type	m_eType;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::WaitForReady(const GfRenderContext& kCtx)
{
	WaitForReadyPlatform(kCtx);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::Submit(
	const GfRenderContext& kCtx,
	GfRencerContextFamilies::Type eQueueType,
	Bool bLast)
{
	SubmitPlatform(kCtx, eQueueType, bLast);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFER_H__