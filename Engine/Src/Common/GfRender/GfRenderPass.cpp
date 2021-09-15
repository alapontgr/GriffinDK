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
	, m_depthAttachment()
	, m_hash(0)
	, m_clearColor(0.0f)
	, m_depthClear(0.0f)
	, m_stencilClear(0)
	, m_width(0)
	, m_height(0)
	, m_offsetX(0)
	, m_offsetY(0)
{
}

////////////////////////////////////////////////////////////////////////////////

struct HashableAttachmentEntry 
{
	u16 m_sampleCount;
	TextureFormat::Type m_format;
	LoadOp m_loadOp;
	StoreOp m_storeOp;
	LoadOp m_stencilLoadOp;
	StoreOp m_stencilStoreOp;
};

void GfRenderPass::setAttachments(const AttachmentDesc* output, u32 outputCount, const AttachmentDesc* depthAttachment)
{
	m_depthAttachment.m_attachment = nullptr;
	if (depthAttachment && depthAttachment->m_attachment) 
	{
		m_depthAttachment = *depthAttachment;
		GF_ASSERT(m_depthAttachment.m_attachment->isDepthBuffer(), "Invalid usage");
	}
	
	m_outputCount = outputCount;
	m_attachments.clear();
	m_attachments.reserve(outputCount);
	for (u32 i=0; i<outputCount; ++i) 
	{
		m_attachments[i] = output[i];
		GF_ASSERT(m_attachments[i].m_attachment->isRT(), "Invalid usage");
	}

	updateHash();
	m_kPlatform.markAsChanged();
}

void GfRenderPass::updateHash() 
{
	GF_ASSERT_ALWAYS("TODO: Implement me");
	// To consider for hash:
	// 1. Width and height
	// 2. Depth attachment reference (HashableAttachmentEntry)
	// 3. Color attachments (HashableAttachmentEntry)
}

////////////////////////////////////////////////////////////////////////////////
// EOF