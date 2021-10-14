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

	VkRenderPass getRenderPass() const { return m_renderPass; }

	VkFramebuffer getFramebuffer() const { return m_framebuffer; }

	void getOrCreateRenderPass(const GfRenderContext& ctx);

protected:

	void markAsChanged();

private:

	VkRenderPass createRenderPass(const GfRenderContext& ctx);

	VkFramebuffer getCreateFramebuffer(const GfRenderContext& ctx);

	VkFramebuffer	m_framebuffer;
	VkRenderPass	m_renderPass;

	static GfMutex ms_renderPasscacheMutex;
	static GfMutex ms_framebuffercacheMutex;
	static GfUMap<u64, VkRenderPass> ms_renderPassCache;
	static GfUMap<u64, VkFramebuffer> ms_framebufferCache;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERPASS_PLATFORM_H__