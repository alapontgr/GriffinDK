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
#include "Common/GfRender/GfRenderCommon.h"
#include "Common/GfCore/GfMaths.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GfRenderPass_Platform.h)

////////////////////////////////////////////////////////////////////////////////

class GfTextureView;

enum class LoadOp : u8
{
	Load = 0,
	Clear,
	DontCare
};

enum class StoreOp : u8
{
	Store = 0,
	DontCare
};

////////////////////////////////////////////////////////////////////////////////

struct AttachmentDesc 
{
	GfTextureView* m_attachment;
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

	void setAttachments(const AttachmentDesc* output, u32 outputCount, const AttachmentDesc* depthAttachment);

	void setAttachments(const AttachmentDesc* output, u32 outputCount, const v4& clearColor,
		const AttachmentDesc* depthAttachment, f32 clearDepth = 0.0f, u32 clearStencil = 0);

	void setViewport(const GfViewport& kViewport);

	void setScissor(const GfScissor& kScissor);
	
	void setRenderArea(u32 offsetX, u32 offsetY, u32 width, u32 height);

	void setClearColor(const v4& clearColor, f32 clearDepth = 0.0f, u32 clearStencil = 0);

	////////////////////////////////////////////////////////////////////////////////

	u64 getHash() const { GF_ASSERT(m_hash, "Hash was not calculated, something went wrong"); return m_hash; }

	bool usesDepthAttachment() const { return m_depthAttachment.m_attachment != nullptr; }

	u32 getAttachmentCount() const { return m_outputCount; }

	f32 getDepthClear() const { return m_depthClear; }

	u32 getStencilClear() const { return m_stencilClear; }

	v4 getClearColor() const { return m_clearColor; }

	u32 getOffsetX() const { return m_offsetX; }

	u32 getOffsetY() const { return m_offsetY; }

	u32 getWidth() const { return m_width; }

	u32 getHeight() const { return m_height; }

	GfViewport getViewport() const { return m_viewport; }

	GfScissor getScissor() const { return m_scissor; }

private:

	static constexpr u32 s_MAX_ATTACHMENTS = 8;

	void updateHash();

	u32 m_outputCount;
	AttachmentDesc m_depthAttachment;
	GfVector<AttachmentDesc> m_attachments;
	u64 m_hash;

	GfViewport m_viewport;
	GfScissor m_scissor;
	v4 m_clearColor;
	f32 m_depthClear;
	u32 m_stencilClear;
	u32 m_width;
	u32 m_height;
	u32 m_offsetX;
	u32 m_offsetY;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfRenderPass::setViewport(const GfViewport& viewport)
{
	m_viewport = viewport;
}

GF_FORCEINLINE void GfRenderPass::setScissor(const GfScissor& scissor)
{
	m_scissor = scissor;
}

GF_FORCEINLINE void GfRenderPass::setRenderArea(u32 offsetX, u32 offsetY, u32 width, u32 height) 
{
	m_offsetX = offsetX;
	m_offsetY = offsetY;
	m_width = width;
	m_height = height;
	m_kPlatform.markAsChanged();
}

GF_FORCEINLINE void GfRenderPass::setClearColor(const v4& clearColor, f32 clearDepth, u32 clearStencil) 
{
	m_clearColor = clearColor;
	m_depthClear = clearDepth;
	m_stencilClear = clearStencil;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERPASS_H__