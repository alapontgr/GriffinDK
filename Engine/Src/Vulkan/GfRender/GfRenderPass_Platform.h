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

#include "Common/GfRender/GfGraphicsSDK.h"
#include "Common/GfRender/GfRenderConstants.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;
class GfCmdBuffer;
struct GfViewport;
struct GfScissor;
struct AttachmentDesc;

////////////////////////////////////////////////////////////////////////////////

class GfWindow;

////////////////////////////////////////////////////////////////////////////////

class GfRenderPass_Platform 
{
	GF_DECLARE_PLATFORM_MEMBERS(GfRenderPass);
public:

	VkRenderPass GetRenderPass() const { return m_renderPass; }

protected:

	void createRHI(const GfRenderContext& ctx, const GfWindow* window);

	bool createRHI(const GfRenderContext& ctx,
		const AttachmentDesc* output, u32 outputCount,
		const AttachmentDesc* depthAttachment);

	////////////////////////////////////////////////////////////////////////////////

	void BeginPassRHI(const GfRenderContext& ctx, const GfCmdBuffer& kCmdBuffer, const GfWindow* pWindow);

	void EndPassRHI(const GfCmdBuffer& cmdBuffer);

	void SetViewportRHI(const GfCmdBuffer& cmdBuffer, const GfViewport& viewport);

	void SetScissorRHI(const GfCmdBuffer& cmdBuffer, const GfScissor& scissors);

	////////////////////////////////////////////////////////////////////////////////

private:

	VkRenderPass createRenderPass(const GfRenderContext& ctx,
		const AttachmentDesc* output, u32 outputCount,
		const AttachmentDesc* depthAttachment);

	VkFramebuffer getOrCreateFramebuffer(const GfRenderContext& ctx,
		const AttachmentDesc* output, u32 outputCount,
		const AttachmentDesc* depthAttachment);

public:
	VkFramebuffer	m_framebuffer;
	VkRenderPass	m_renderPass;

	static GfUMap<u64, VkRenderPass> ms_renderPassCache;
	static GfUMap<u64, VkFramebuffer> ms_framebufferCache;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERPASS_PLATFORM_H__