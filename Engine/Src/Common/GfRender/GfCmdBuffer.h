////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/16
//	File: 	GfCmdBuffer.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFCMDBUFFER_H__
#define __GFCMDBUFFER_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfCore/GfCoreMinimal.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GfCmdBuffer_Platform.h)
#include "Common/GfRender/GfShaderPipeline.h"

////////////////////////////////////////////////////////////////////////////////

namespace GfCmdBufferType
{
	enum Type : u32
	{
		Invalid,
		Primary,
		Secondary
	};
}

////////////////////////////////////////////////////////////////////////////////

namespace GfRenderStateFlags 
{
	enum Type : u32 
	{
		BindMaterial = 1<<0
	};
}

struct GfShaderPipeState 
{
	GfShaderPipeline* m_curPipeline = nullptr;
	const GfShaderVariantData* m_curVariant = nullptr;
	GfVariantHash m_curVariantHash = 0;
};

class GfRenderPipelineState 
{
public:

	u64 getHash() const { return m_pipelineHash; }

	GfBlendState getBlendState() const { return m_config.m_blendState; }
	GfRasterState getRasterState() const { return m_config.m_rasterState; }
	PrimitiveTopology::Type getTopology() const { return m_config.m_topology; }
	GfMultiSamplingState getMsState() const { return m_config.m_msaState; }
	GfDepthState getDepthState() const { return m_config.m_depthState; }
	const GfVertexDeclaration* getVertexFormat() const { return m_curVertexFormat; }
	GfRenderPass* getCurRenderPass() const { return m_curRenderPass; }
	GfShaderPipeline* getActivePipeline() const { return m_curShaderState.m_curPipeline; }
	GfVariantHash getActiveShaderVariant() const { return m_curShaderState.m_curVariantHash; }
	const GfShaderVariantData* getActiveVariantData() const { return m_curShaderState.m_curVariant; }
	u32 getFlags() const { return m_stateFlags; }

	const GfShaderPipeConfig& getConfig() const { return m_config; }

	void setBlendState(const GfBlendState& blendState) { m_config.m_blendState = blendState; updateHash(); }
	void setRasterState(const GfRasterState& rasterState) { m_config.m_rasterState = rasterState; updateHash(); }
	void setTopology(PrimitiveTopology::Type topology) { m_config.m_topology = topology; updateHash(); }
	void setMSAAState(const GfMultiSamplingState& msaaState) { m_config.m_msaState = msaaState; updateHash(); }
	void setDepthState(const GfDepthState& depthState) { m_config.m_depthState = depthState; updateHash(); }
	void setVertexFormat(const GfVertexDeclaration* vertexFormat) { m_curVertexFormat = vertexFormat; updateHash(); }
	void setRenderPass(GfRenderPass* renderPass) { m_curRenderPass = renderPass; updateHash(); }
	void setShaderPipe(GfShaderPipeline* pipeline, const GfVariantHash variantHash) 
	{
		m_curShaderState.m_curPipeline = pipeline;
		m_curShaderState.m_curVariantHash = variantHash;
		m_curShaderState.m_curVariant =  pipeline->getDeserializer().getVariantData(variantHash);
		enableFlag(GfRenderStateFlags::BindMaterial);
		updateHash();
	}
	void enableFlag(GfRenderStateFlags::Type flag) 
	{
		m_stateFlags |= flag;
	}
	void disableFlag(GfRenderStateFlags::Type flag) 
	{
		m_stateFlags &= ~flag;
	}


private:
	void updateHash();

	const GfVertexDeclaration* m_curVertexFormat = nullptr;
	GfRenderPass* m_curRenderPass = nullptr;
	GfShaderPipeState m_curShaderState;
	GfShaderPipeConfig m_config;
	u32 m_stateFlags = 0;

	u64 m_pipelineHash = 0;
};

class GfBuffer;

struct GfTextureBarrier 
{
	GfTextureView* m_view = nullptr;
	TextureUsageFlags::Mask m_oldUsage;
	TextureUsageFlags::Mask m_newUsage;
};
struct GfBufferBarrier 
{
	GfBuffer* m_buffer = nullptr;
	u32 m_offset = 0;
	u32 m_size;
	BufferUsageFlags::Mask m_oldUsage;
	BufferUsageFlags::Mask m_newUsage;
};

class GfCmdBuffer
{
	GF_DECLARE_PLATFORM_INTERFACE(GfCmdBuffer);
public:

	friend class GfCmdBuffer_Platform;
	friend class GfCmdBufferCache;

	static GfCmdBuffer* get(const GfRenderContext* ctx, 
		GfCmdBufferType::Type type,
		GfRenderContextFamilies::Type queue);

	////////////////////////////////////////////////////////////////////////////////
	// CmdBuffer commands

	void beginRecording();

	void endRecording();

	bool isReady();

	// Sync point to avoid start recording while the command buffer is still being processed
	void waitForReady();

	void submit(const GfWindow& kWindow, Bool bLast);

	////////////////////////////////////////////////////////////////////////////////

	void setBlendState(const GfBlendState& blendState);

	void setRasterState(const GfRasterState& rasterState);

	void setTopology(PrimitiveTopology::Type topology);

	void setMSAAState(const GfMultiSamplingState& msaaState);

	void setVertexFormat(const GfVertexDeclaration* vertexFormat);

	void setDepthState(const GfDepthState& depthState);
	
	void setStencilCompareMask(StencilFace::Type targetFace, u32 compareMask);

	void setStencilWriteMask(StencilFace::Type targetFace, u32 writeMask);

	void setStencilReferenceValue(StencilFace::Type targetFace, u32 refVal);

	////////////////////////////////////////////////////////////////////////////////

	void beginRenderPass(GfRenderPass* renderPass);
	
	void endRenderPass();

	void bindShaderPipe(const class GfShaderVariant& shaderVariant);

	void bindVertexBuffers(GfBuffer** vertexBuffers, u32* vertexBufferOffsets, u32 vertexBufferCount);
	
	void bindIndexBuffer(const GfBuffer& buffer, u32 offset, bool useShort);

	// Resource binding

	void bindUniformBuffer(const u32 setIdx, const u32 bindIdx, const GfBuffer& buffer, const u32 offset, const u32 size);

	void bindSampledTexture(const u32 setIdx, const u32 bindIdx, GfTextureView* texture, const u32 arrayIdx = 0);

	void bindStorageImage(const u32 setIdx, const u32 bindIdx, GfTextureView* texture, const u32 arrayIdx = 0);

	void bindSampler(const u32 setIdx, const u32 bindIdx, const GfSampler& sampler);

	// Sync

	void addTextureBarrier(GfTextureView* texture, TextureUsageFlags::Mask newUsage);

	void addBufferBarrier(GfBuffer* buffer, u32 offset, u32 size, BufferUsageFlags::Mask newUsage);

	// Drawcalls

	void drawIndexed(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset = 0, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

	void draw(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex = 0, u32 uiFirstInstance = 0);

private:

	static constexpr u32 ms_allocatorDefaultMemory = 16 * 1024; // 16 KB

	GfCmdBuffer();

	GfCmdBuffer(const GfRenderContext* ctx, GfCmdBufferCache* cache,
		const GfCmdBufferType::Type type, GfRenderContextFamilies::Type queue);

	void reset();

	void shutdown();

	GfCmdBufferType::Type	m_type;
	GfRenderContextFamilies::Type m_queue;
	const GfRenderContext* m_ctx;
	class GfCmdBufferCache* m_cache;
	GfLinearAllocator m_linearAllocator;
	GfRenderPipelineState m_curState;
};

////////////////////////////////////////////////////////////////////////////////

class GfCmdBufferFactory
{
	GF_DECLARE_PLATFORM_INTERFACE(GfCmdBufferFactory);
public:

	GfCmdBufferFactory();

	void init(const GfRenderContext& kCtx, GfRenderContextFamilies::Type eQueueType);

	void shutdown(const GfRenderContext& ctx);

	bool isInitialized() const { return m_initialized; }

private:

	bool m_initialized;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::waitForReady()
{
	m_kPlatform.waitForReadyRHI(*m_ctx);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::beginRecording()
{
	m_kPlatform.beginRecordingRHI(*m_ctx);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfCmdBuffer::endRecording()
{
	m_kPlatform.endRecordingRHI(*m_ctx);
}

GF_FORCEINLINE bool GfCmdBuffer::isReady()
{
	return m_kPlatform.isReady(*m_ctx);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFER_H__