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
	GfTextureFormat::Type m_format;
	LoadOp m_loadOp;
	StoreOp m_storeOp;
	LoadOp m_stencilLoadOp;
	StoreOp m_stencilStoreOp;
};

void GfRenderPass::setAttachments(const AttachmentDesc* output, u32 outputCount, const AttachmentDesc* depthAttachment)
{
	setAttachments(output, outputCount, v4(0.0f), depthAttachment);
}

void GfRenderPass::setAttachments(const AttachmentDesc* output, u32 outputCount, const v4& clearColor,
	const AttachmentDesc* depthAttachment, f32 clearDepth, u32 clearStencil) 
{
	m_depthAttachment.m_attachment = GfTextureView();
	if (depthAttachment && depthAttachment->m_attachment.getTexture()) 
	{
		m_depthAttachment = *depthAttachment;
		GF_ASSERT(m_depthAttachment.m_attachment.getTexture()->isDepthBuffer(), "Invalid usage");
	}
	
	m_outputCount = outputCount;
	m_attachments.clear();
	m_attachments.resize(outputCount);
	for (u32 i=0; i<outputCount; ++i) 
	{
		m_attachments[i] = output[i];
		GF_ASSERT(m_attachments[i].m_attachment.getTexture()->isRT(), "Invalid usage");
	}

	m_clearColor = clearColor;
	m_depthClear = clearDepth;
	m_stencilClear = clearStencil;

	updateHash();
	m_kPlatform.markAsChanged();
}

void GfRenderPass::updateHash() 
{
	m_hash = 0;
	u32 attachmentCount = m_outputCount + (usesDepthAttachment() ? 1 : 0);
	u32 reqSize = static_cast<u32>(attachmentCount * sizeof(HashableAttachmentEntry));
	StackMemBlock block(reqSize);
	HashableAttachmentEntry* attachmmentDesc = reinterpret_cast<HashableAttachmentEntry*>(block.get());
	HashableAttachmentEntry* pivot = attachmmentDesc;
	if (usesDepthAttachment())
	{
		pivot->m_sampleCount = 1; // TODO MSAA
		pivot->m_format = m_depthAttachment.m_attachment.getTexture()->getFormat();
		pivot->m_loadOp = m_depthAttachment.m_loadOp;
		pivot->m_storeOp = m_depthAttachment.m_storeOp;
		pivot->m_stencilLoadOp = m_depthAttachment.m_stencilLoadOp;
		pivot->m_stencilStoreOp = m_depthAttachment.m_stencilStoreOp;
		pivot++;
	}
	for (u32 i = 0; i < m_outputCount; ++i)
	{
		pivot->m_sampleCount = 1; // TODO MSAA
		pivot->m_format = m_attachments[i].m_attachment.getTexture()->getFormat();
		pivot->m_loadOp = m_attachments[i].m_loadOp;
		pivot->m_storeOp = m_attachments[i].m_storeOp;
		pivot->m_stencilLoadOp = m_attachments[i].m_stencilLoadOp;
		pivot->m_stencilStoreOp = m_attachments[i].m_stencilStoreOp;
		pivot++;
	}
	m_hash = GfHash::compute(attachmmentDesc, reqSize);
}

////////////////////////////////////////////////////////////////////////////////
// EOF