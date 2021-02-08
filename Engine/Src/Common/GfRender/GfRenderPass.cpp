////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/17
//	File: 	GfRenderPass.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GfRenderPass.h"
#include "Common/GfRender/GraphicResources/GfGraphicResources.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfRenderPass)
	, m_outputCount(0)
	, m_depthAttachment(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderPass::Create(const GfRenderContext& ctx, const GfWindow* pWindow)
{
	m_kPlatform.createRHI(ctx, pWindow);
	m_kPlatform.RecreateFramebufferRHI(ctx, pWindow);
}

bool GfRenderPass::create(const GfRenderContext& ctx,
	const AttachmentDesc* output, u32 outputCount, 
	const AttachmentDesc* depthAttachment)
{
	if (depthAttachment) 
	{
		m_depthAttachment = depthAttachment->m_attachment;
		GF_ASSERT(m_depthAttachment->isDepthBuffer(), "Invalid usage");
	}
	m_outputCount = outputCount;
	for (u32 i=0; i<outputCount; ++i) 
	{
		m_attachments[i] = output[i].m_attachment;
		GF_ASSERT(m_attachments[i]->isRT(), "Invalid usage");
	}
	return m_kPlatform.createRHI(ctx, output, outputCount, depthAttachment);
}

////////////////////////////////////////////////////////////////////////////////
// EOF