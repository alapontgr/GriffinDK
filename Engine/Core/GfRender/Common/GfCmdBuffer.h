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

class GfCmdBuffer
{
	GF_DECLARE_PLATFORM_INTERFACE(GfCmdBuffer);
public:

	friend class GfCmdBuffer_Platform;

	GfCmdBuffer();

	void Init(GfCmdBufferType::Type eType);

	////////////////////////////////////////////////////////////////////////////////
	// CmdBuffer commands

	void BeginRecording(const GfRenderContext& kCtx);

	void EndRecording(const GfRenderContext& kCtx);

	// Sync point to avoid start recording while the command buffer is still being processed
	void WaitForReady(const GfRenderContext& kCtx);

	void Submit(
		const GfRenderContext& kCtx,
		GfRenderContextFamilies::Type eQueueType,
		Bool bLast);


	////////////////////////////////////////////////////////////////////////////////
	// Render pipeline state

	// Bind a parameter set to a slot
	void BindParameterSet(const GfMaterialTemplate& kMaterial, const GfMaterialParamSet& kparamSet, 
		u32 uiBindPoint = 0, bool bIsGraphics = true);

	////////////////////////////////////////////////////////////////////////////////
	// Draw commands

	void DrawIndexed(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset = 0, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

	void Draw(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

	////////////////////////////////////////////////////////////////////////////////

private:

	GfCmdBufferType::Type	m_eType;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::WaitForReady(const GfRenderContext& kCtx)
{
	m_kPlatform.WaitForReadyRHI(kCtx);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::Submit(
	const GfRenderContext& kCtx,
	GfRenderContextFamilies::Type eQueueType,
	Bool bLast)
{
	m_kPlatform.SubmitRHI(kCtx, eQueueType, bLast);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::BeginRecording(const GfRenderContext& kCtx)
{
	m_kPlatform.BeginRecordingRHI(kCtx);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::EndRecording(const GfRenderContext& kCtx)
{
	m_kPlatform.EndRecordingRHI(kCtx);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::BindParameterSet(
	const GfMaterialTemplate& kMaterial, const GfMaterialParamSet& kparamSet, 
	u32 uiBindPoint, bool bIsGraphics)
{
	m_kPlatform.BindParameterSetRHI(kMaterial, kparamSet, uiBindPoint, bIsGraphics);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::DrawIndexed(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset /*= 0*/, u32 uiVertexOffset /*= 0*/, u32 uiFirstInstanceId /*= 0*/)
{
	m_kPlatform.DrawIndexedRHI(uiIdxCount, uiInstanceCount, uiIdxOffset, uiVertexOffset, uiFirstInstanceId);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::Draw(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex /*= 0*/, u32 uiFirstInstance /*= 0*/)
{
	m_kPlatform.DrawRHI(uiVertexCount, uiInstanceCount, uiFirstVertex, uiFirstInstance);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFER_H__