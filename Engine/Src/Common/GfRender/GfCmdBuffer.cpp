////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/16
//	File: 	GFCMDBUFFER.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GfCmdBuffer.h"
#include "Common/GfRender/GfShaderPipeline.h"
#include "Common/GfRender/GfRenderPass.h"

#include <mutex>

////////////////////////////////////////////////////////////////////////////////

class GfCmdBufferCache 
{
public:
	GfCmdBufferCache() {}

	GfCmdBuffer* get(const GfRenderContext* ctx, GfCmdBufferType::Type type, GfRenderContextFamilies::Type queue);

	void returnToCache(GfCmdBuffer* cmdBuffer);

private:

	GfCmdBufferFactory m_factory;
	GfVector<GfUniquePtr<GfCmdBuffer>> m_createdCmdBuffers;
	GfQueue<GfCmdBuffer*> m_avalCmdBuffers;
	std::mutex m_cacheMutex;
};

GfCmdBuffer* GfCmdBufferCache::get(const GfRenderContext* ctx, GfCmdBufferType::Type type, GfRenderContextFamilies::Type queue)
{
	std::lock_guard<std::mutex> lk(m_cacheMutex);
	
	if (!m_factory.isInitialized()) 
	{
		m_factory.init(*ctx, queue);
	}

	if (m_avalCmdBuffers.size() > 0) 
	{
		GfCmdBuffer* buffer = m_avalCmdBuffers.front();
		if (buffer->isReady()) 
		{
			m_avalCmdBuffers.pop();
			buffer->reset();
			return buffer;
		}
	}

	// If no CmdBuffer was found, allocate a new one
	GfUniquePtr<GfCmdBuffer> cmdBuffer = GfUniquePtr<GfCmdBuffer>(new GfCmdBuffer(ctx, this, type, queue));
	m_createdCmdBuffers.push_back(std::move(cmdBuffer));
	GfCmdBuffer* buffer = m_createdCmdBuffers.back().get();
	buffer->reset();
	return buffer;
}

void GfCmdBufferCache::returnToCache(GfCmdBuffer* cmdBuffer) 
{
	GF_ASSERT(m_factory.isInitialized(), "Factory was not initialized, how did this happen?");
	std::lock_guard<std::mutex> lk(m_cacheMutex);
	m_avalCmdBuffers.push(cmdBuffer);
}

////////////////////////////////////////////////////////////////////////////////

GfCmdBufferCache s_cmdBufferCaches[GfRenderContextFamilies::Count];

////////////////////////////////////////////////////////////////////////////////

GfCmdBufferFactory::GfCmdBufferFactory()
	: m_initialized(false)
{
}

void GfCmdBufferFactory::init(const GfRenderContext& kCtx, GfRenderContextFamilies::Type eQueueType)
{
	m_initialized = true;
	m_kPlatform.initRHI(kCtx, eQueueType);
}

void GfCmdBufferFactory::shutdown(const GfRenderContext& ctx)
{
	m_initialized = false;
	m_kPlatform.shutdown(ctx);
}

////////////////////////////////////////////////////////////////////////////////

GfCmdBuffer* GfCmdBuffer::get(const GfRenderContext* ctx, 
		GfCmdBufferType::Type type,
		GfRenderContextFamilies::Type queue)
{
	GfCmdBufferCache& cache = s_cmdBufferCaches[queue];
	GfCmdBuffer* res = cache.get(ctx, type, queue);
	GF_ASSERT(res, "Failed to allocate CmdBuffer");
	return res;
}

GfCmdBuffer::GfCmdBuffer()
	: m_type(GfCmdBufferType::Invalid)
	, m_queue(GfRenderContextFamilies::InvalidIdx)
	, m_ctx(nullptr)
	, m_cache(nullptr)
{
}

GfCmdBuffer::GfCmdBuffer(const GfRenderContext* ctx, GfCmdBufferCache* cache,
	const GfCmdBufferType::Type type, GfRenderContextFamilies::Type queue)
	: m_type(type)
	, m_queue(queue)
	, m_ctx(ctx)
	, m_cache(cache)
{
}

void GfCmdBuffer::reset() 
{
	GF_ASSERT(m_ctx, "Context was not initialized");
	m_curState = GfRenderPipelineState();
}

void GfCmdBuffer::drawcallCommon() 
{
	GF_ASSERT(m_curState.m_curVertexFormat, "A vertex format was not provided");
	GF_ASSERT(m_curState.m_curRenderPass, "A RenderPass needs to be active");
	GF_ASSERT(m_type == GfCmdBufferType::Primary, "TODO: Add support for secondary command buffer recording");
	
	// Bind pipeline for variant
	m_kPlatform.bindShaderPipe(m_curState.m_curPipeline, m_curState.m_curVariantHash, 
		&m_curState.m_config, m_curState.m_configHash,
		m_curState.m_curVertexFormat, m_curState.m_curRenderPass);

	// Bind batch resources. Bind & Update descriptor sets

}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer::setBlendState(const GfBlendState& blendState) 
{
	m_curState.m_config.m_blendState = blendState;
}

void GfCmdBuffer::setRasterState(const GfRasterState& rasterState) 
{
	m_curState.m_config.m_rasterState = rasterState;
}

void GfCmdBuffer::setTopology(PrimitiveTopology::Type topology) 
{
	m_curState.m_config.m_topology = topology;
}

void GfCmdBuffer::setMSAAState(const GfMultiSamplingState& msaaState) 
{
	m_curState.m_config.m_msaState = msaaState;
}

void GfCmdBuffer::setVertexFormat(const GfVertexDeclaration* vertexFormat) 
{
	m_curState.m_curVertexFormat = vertexFormat;
}

void GfCmdBuffer::setDepthState(const GfDepthState& depthState) 
{
	m_curState.m_config.m_depthState = depthState;
	m_curState.m_configHash = GfHash::compute(&m_curState.m_config, sizeof(GfShaderPipeConfig));
}

void GfCmdBuffer::setStencilCompareMask(StencilFace::Type targetFace, u32 compareMask) 
{
	GF_ASSERT_ALWAYS("TODO: Implement me!");
}

void GfCmdBuffer::setStencilWriteMask(StencilFace::Type targetFace, u32 writeMask) 
{
	GF_ASSERT_ALWAYS("TODO: Implement me!");
}

void GfCmdBuffer::setStencilReferenceValue(StencilFace::Type targetFace, u32 refVal) 
{
	GF_ASSERT_ALWAYS("TODO: Implement me!");
}

void GfCmdBuffer::beginRenderPass(GfRenderPass* renderPass)
{
	GF_ASSERT(!m_curState.m_curRenderPass, "Trying to begin a pass before end");
	GF_ASSERT(renderPass->getWidth() != 0 && renderPass->getHeight(), "Resolution was not initialized");

	m_curState.m_curRenderPass = renderPass;
	m_kPlatform.setViewport(*m_ctx, renderPass->getViewport());
	m_kPlatform.setScissors(*m_ctx, renderPass->getScissor());
	m_kPlatform.beginRenderPass(*m_ctx, renderPass);
}

void GfCmdBuffer::endRenderPass()
{
	GF_ASSERT(m_curState.m_curRenderPass, "Trying to end an invalid pass");
	m_kPlatform.endRenderPass();
	m_curState.m_curRenderPass = nullptr;
}

void GfCmdBuffer::bindShaderPipe(const GfShaderVariant& shaderVariant)
{ 
	GfShaderPipeline* pipeline = shaderVariant.getPipeline();
	GfVariantHash variantHash = shaderVariant.getVariantHash();

	GF_ASSERT(pipeline->isCompute() || (!pipeline->isCompute() && m_curState.m_curVertexFormat), "Invalid active vertex format");
	GF_ASSERT(pipeline->isCompute() || (!pipeline->isCompute() && m_curState.m_curRenderPass), "Invalid active Render Pass");

	m_curState.m_curPipeline = pipeline;
	m_curState.m_curVariantHash = variantHash;
}

void GfCmdBuffer::submit(
	const GfWindow& kWindow,
	Bool bLast)
{
	m_kPlatform.submitRHI(*m_ctx, kWindow, m_queue, bLast);
	GF_ASSERT(m_cache, "Invalid cache");
	m_cache->returnToCache(this);
}

void GfCmdBuffer::drawIndexed(u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset /*= 0*/, u32 uiVertexOffset /*= 0*/, u32 uiFirstInstanceId /*= 0*/)
{
	drawcallCommon();
	m_kPlatform.drawIndexedRHI(uiIdxCount, uiInstanceCount, uiIdxOffset, uiVertexOffset, uiFirstInstanceId);
}

void GfCmdBuffer::draw(u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex /*= 0*/, u32 uiFirstInstance /*= 0*/)
{
	drawcallCommon();
	m_kPlatform.drawRHI(uiVertexCount, uiInstanceCount, uiFirstVertex, uiFirstInstance);
}

void GfCmdBuffer::bindVertexBuffers(GfBuffer** vertexBuffers, u32* vertexBufferOffsets, u32 vertexBufferCount) 
{	
	m_kPlatform.bindVertexBuffersRHI(vertexBuffers, vertexBufferOffsets, vertexBufferCount);
}

void GfCmdBuffer::bindIndexBuffer(const GfBuffer& buffer, u32 offset, bool useShort)
{
	m_kPlatform.bindIndexBufferRHI(buffer, offset, useShort);
}

////////////////////////////////////////////////////////////////////////////////
// EOF
