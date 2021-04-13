////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/17
//	File: 	GfRenderPass.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFRENDERPASS_H__
#define __GFRENDERPASS_H__
////////////////////////////////////////////////////////////////////////////////
// Include

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfCore/GfMaths.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GfRenderPass_Platform.h)

////////////////////////////////////////////////////////////////////////////////

class GfTexture;

enum class LoadOp : u32
{
	Load = 0,
	Clear,
	DontCare
};

enum class StoreOp : u32
{
	Store = 0,
	DontCare
};

////////////////////////////////////////////////////////////////////////////////

struct AttachmentDesc 
{
	const GfTexture* m_attachment;
	LoadOp m_loadOp = LoadOp::Load;
	StoreOp m_storeOp = StoreOp::Store;
	LoadOp m_stencilLoadOp = LoadOp::DontCare;
	StoreOp m_stencilStoreOp = StoreOp::DontCare;
};

////////////////////////////////////////////////////////////////////////////////

class GfRenderPass
{
	GF_DECLARE_PLATFORM_INTERFACE(GfRenderPass);
public:

	GfRenderPass();

	bool create(const GfRenderContext& ctx,
		const AttachmentDesc* output, u32 outputCount,
		const AttachmentDesc* depthAttachment);

	////////////////////////////////////////////////////////////////////////////////
	// Commands

	void BeginPass(const GfRenderContext& ctx, const GfCmdBuffer& kCmdBuffer, const GfWindow* pWindow);

	void EndPass(const GfCmdBuffer& kCmdBuffer);

	void SetViewport(const GfCmdBuffer& kCmdBuffer, const GfViewport& kViewport);

	void SetScissor(const GfCmdBuffer& kCmdBuffer, const GfScissor& kScissor);
	
	////////////////////////////////////////////////////////////////////////////////

private:

	static constexpr u32 s_MAX_ATTACHMENTS = 8;

	u16 m_outputCount;
	const GfTexture* m_depthAttachment;
	GfArray<const GfTexture*, s_MAX_ATTACHMENTS> m_attachments;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfRenderPass::BeginPass(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfWindow* pWindow)
{
	m_kPlatform.BeginPassRHI(kCtx, kCmdBuffer, pWindow);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfRenderPass::EndPass(const GfCmdBuffer& kCmdBuffer)
{
	m_kPlatform.EndPassRHI(kCmdBuffer);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfRenderPass::SetViewport(const GfCmdBuffer& kCmdBuffer, const GfViewport& kViewport)
{
	m_kPlatform.SetViewportRHI(kCmdBuffer, kViewport);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfRenderPass::SetScissor(const GfCmdBuffer& kCmdBuffer, const GfScissor& kScissor)
{
	m_kPlatform.SetScissorRHI(kCmdBuffer, kScissor);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERPASS_H__