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

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfCore/Common/GfMaths.h"
#include GF_SOLVE_PLATFORM_HEADER(GfRenderPass)

////////////////////////////////////////////////////////////////////////////////

namespace GfRenderPassClear 
{
	enum Type : u32 
	{
		Clear,
		// TODO
	};
}

////////////////////////////////////////////////////////////////////////////////

class GfRenderPass
{
	GF_DECLARE_PLATFORM_INTERFACE(GfRenderPass);
public:

	GfRenderPass();

	void AddInputSlot();

	void AddOutputSlot();

	void SetOutputClearMode(GfRenderPassClear::Type eMode, v4 vClearColor = v4(0.0f));

	void Create(const GfRenderContext& kCtx, const GfWindow* pWindow);

	void Resize(const GfRenderContext& kCtx);

	////////////////////////////////////////////////////////////////////////////////
	// Commands

	void BeginPass(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfWindow* pWindow);

	void EndPass(const GfCmdBuffer& kCmdBuffer);

	void SetViewport(const GfCmdBuffer& kCmdBuffer, const GfViewport& kViewport);

	void SetScissor(const GfCmdBuffer& kCmdBuffer, const GfScissor& kScissor);
	
	////////////////////////////////////////////////////////////////////////////////

private:
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