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

#include "Common/GfCore/GfMaths.h"
#include "Common/GfRender/GfGraphicsSDK.h"
#include "Common/GfRender/GfRenderConstants.h"
#include "Common/GfRender/GfShaderPipeline.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;
class GfWindow;
class GfRenderPass;
class GfBuffer;
class GfTextureView;
class GfSampler;
class GfSemaphore;
struct GfViewport;
struct GfScissor;
struct GfRenderPipelineState;
struct GfTextureBarrier;
struct GfBufferBarrier;

////////////////////////////////////////////////////////////////////////////////

struct GfResourceBindingEntry 
{
	struct BufferBinding 
	{
		VkBuffer m_buffer;
		u32 m_offset;
		u32 m_size;
	};
	struct SamplerBinding 
	{
		VkSampler m_sampler;
	};
	struct SampledTextureBinding 
	{
		VkImageView m_view;
		VkImageLayout m_layout;
	};
	struct ImageBinding 
	{
		VkImageView m_view;
		VkImageLayout m_layout;
	};
	struct CombinedSamplerTexture 
	{
		VkImageView m_view;
		VkSampler m_sampler;
		VkImageLayout m_layout;
	};

	union 
	{
		BufferBinding m_bufferBind;
		SamplerBinding m_samplerBind;
		SampledTextureBinding m_sampledTextureBind;
		ImageBinding m_imageBind;
		CombinedSamplerTexture m_combinedSamplerTextureBind;
	};
	GfParameterSlotType::Type m_type;
};

struct GfResourceBindingExt 
{
	u32 m_arrayCount = 0;
	union 
	{
		GfResourceBindingEntry m_single;
		GfResourceBindingEntry* m_array;
	};
};

class GfCmdBuffer_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfCmdBuffer);
public:

	friend class GfCmdBufferFactory_Platform;

	VkCommandBuffer getCmdBuffer() const { return m_cmdBuffer; }

	VkFence getFence() const { return m_fence; }

private:

	void initRHI(const GfRenderContext& ctx, const class GfCmdBufferFactory& factory, u32 type);

	void reset();

	void shutdown(const GfRenderContext& ctx);

	void waitForReadyRHI(const GfRenderContext& ctx);

	bool isReady(const GfRenderContext& ctx);

	void submitRHI(
		const GfRenderContext& ctx,
		GfRenderContextFamilies::Type eQueueType, 
		const GfSemaphore* waitSemaphore = nullptr,
		const GfSemaphore* signalSemaphore = nullptr);

	////////////////////////////////////////////////////////////////////////////////

	void beginRecordingRHI(const GfRenderContext& kCtx);

	void endRecordingRHI(const GfRenderContext& kCtx);

	////////////////////////////////////////////////////////////////////////////////

	void beginRenderPass(const GfRenderContext& kCtx, GfRenderPass* renderPass);
	
	void endRenderPass();

	void setViewport(const GfRenderContext& ctx, const GfViewport& viewport);

	void setScissors(const GfRenderContext& ctx, const GfScissor& scissor);

	////////////////////////////////////////////////////////////////////////////////

	GfResourceBindingEntry& getEntryForBinding(const u32 setIdx, const u32 bindingIdx);

	void bindUniformBuffer(const u32 setIdx, const u32 bindIdx, const GfBuffer& buffer, const u32 offset, const u32 size);

	void bindSampledTexture(const u32 setIdx, const u32 bindIdx, GfTextureView& texture, const u32 arrayIdx = 0);

	void bindStorageImage(const u32 setIdx, const u32 bindIdx, GfTextureView& texture, const u32 arrayIdx);

	void bindSampler(const u32 setIdx, const u32 bindIdx, const GfSampler& sampler);

	////////////////////////////////////////////////////////////////////////////////

	void flushBarriers(
		const GfWeakArray<GfTextureBarrier>& textureBarriers, 
		const GfWeakArray<GfBufferBarrier>& bufferBarriers);

	////////////////////////////////////////////////////////////////////////////////

	void drawcallCommonGraphics(GfRenderPipelineState* state);

	void drawIndexedRHI(GfRenderPipelineState* state, u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset = 0, u32 uiVertexOffset = 0, u32 uiFirstInstanceId = 0);

	void drawRHI(GfRenderPipelineState* state, u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

	void bindVertexBuffersRHI(GfBuffer** vertexBuffers, u32* vertexBufferOffsets, u32 vertexBufferCount);

	void bindIndexBufferRHI(const GfBuffer& buffer, u32 offset, bool useShort);

	////////////////////////////////////////////////////////////////////////////////

	// Perform Multi buffering of the command buffers to avoid waiting for the end of a previous execution
	VkCommandBuffer m_cmdBuffer;
	VkFence			m_fence;

	// Resources
	GfArray<GfArray<GfResourceBindingExt, s_MAX_BINDINGS_PER_SET>, s_MAX_DESCRIPTOR_SETS> m_bindings;

	// Cur state
	const GfVariantDataVK* m_curBoundVariant;
	VkDescriptorSet m_curBoundSet;
};

class GfCmdBufferFactory_Platform
{
public:

	friend class GfCmdBufferFactory;

	GfCmdBufferFactory_Platform();

	VkCommandPool getPool() const { return m_pool; }

private:

	void initRHI(const GfRenderContext& ctx, GfRenderContextFamilies::Type queueType);

	void shutdown(const GfRenderContext& ctx);

	VkCommandPool m_pool;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFER_PLATFORM_H__