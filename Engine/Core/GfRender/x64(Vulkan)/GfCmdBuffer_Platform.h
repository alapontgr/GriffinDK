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
		GfRenderContextFamilies::Type eQueueType, 
		Bool bLast);

	////////////////////////////////////////////////////////////////////////////////
	// Render pass commands

	void BeginRecordingRHI(const GfRenderContext& kCtx);

	void EndRecordingRHI(const GfRenderContext& kCtx);

	void ClearCurrentTargetRHI(const GfRenderContext& kCtx, const v4& vClearColor);

	void SetViewportRHI(const GfViewport& kViewport);

	void SetScissorRHI(const GfScissor& kScissor);

	////////////////////////////////////////////////////////////////////////////////
	// Buffer commands

	void CopyBufferRangeRHI(const GfRenderContext& kCtx, const GfBuffer& kFrom, const GfBuffer& kTo, u32 uiFromOffset, u32 uiToOffset, u32 uiSize);

	void UpdateBufferRangeRHI(const GfRenderContext& kCtx, const GfBuffer& kBuffer, u32 uiOffset, u32 uiSize, void* pData);

	////////////////////////////////////////////////////////////////////////////////

	void BindMaterialRHI(const GfMaterialTemplate& kMaterial);

	void BindParameterSetRHI(const GfMaterialTemplate& kMaterial, 
		const GfMaterialParamSet& kparamSet, 
		u32 uiBindPoint, bool bIsGraphics);

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