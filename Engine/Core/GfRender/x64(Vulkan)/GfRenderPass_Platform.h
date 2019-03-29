////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/17
//	File: 	GfRenderPass_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFRENDERPASS_PLATFORM_H__
#define __GFRENDERPASS_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfGraphicsSDK.h"
#include "GfRender/Common/GfRenderConstants.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;
class GfCmdBuffer;
struct GfViewport;
struct GfScissor;

////////////////////////////////////////////////////////////////////////////////

class GfRenderPass_Platform 
{
	GF_DECLARE_PLATFORM_MEMBERS(GfRenderPass);
public:

	VkRenderPass GetRenderPass() const { return m_pRenderPass; }

protected:

	void CreateRHI(const GfRenderContext& kCtx);

	////////////////////////////////////////////////////////////////////////////////

	void BeginPassRHI(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer);

	void EndPassRHI(const GfCmdBuffer& kCmdBuffer);

	void SetViewportRHI(const GfCmdBuffer& kCmdBuffer, const GfViewport& kViewport);

	void SetScissorRHI(const GfCmdBuffer& kCmdBuffer, const GfScissor& kScissor);

	void ClearCurrentTargetRHI(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const v4& vClearColor);

	////////////////////////////////////////////////////////////////////////////////

private:

	void RecreateFramebufferRHI(const GfRenderContext& kCtx);

public:
	// TODO: Revise this encapsulation
	VkFramebuffer	m_pFramebuffers[GfRenderConstants::ms_uiNBufferingCount];
	VkRenderPass	m_pRenderPass;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERPASS_PLATFORM_H__