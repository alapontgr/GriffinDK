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

#include <mutex>

////////////////////////////////////////////////////////////////////////////////

struct GfRenderStateDynamic 
{
	GfRenderPass* m_curRenderPass = nullptr;
	GfVertexDeclaration* m_curVertexFormat = nullptr;
	GfShaderPipeline* m_curPipeline = nullptr;
	GfVariantHash m_curVariantHash;
	GfShaderPipeConfig m_config;
};

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
			return buffer;
		}
	}

	// If no CmdBuffer was found, allocate a new one
	GfUniquePtr<GfCmdBuffer> cmdBuffer = GfUniquePtr<GfCmdBuffer>(new GfCmdBuffer(ctx, this, type, queue));
	m_createdCmdBuffers.push_back(std::move(cmdBuffer));
	GfCmdBuffer* buffer = m_createdCmdBuffers.back().get();
	return buffer;
}

void GfCmdBufferCache::returnToCache(GfCmdBuffer* cmdBuffer) 
{
	GF_ASSERT(m_factory.isInitialized(), "Factory was not initialized, how did this happen?");
	std::lock_guard<std::mutex> lk(m_cacheMutex);
	m_avalCmdBuffers.push(cmdBuffer);
}

////////////////////////////////////////////////////////////////////////////////

static thread_local GfRenderStateDynamic s_curState;
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

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer::beginRenderPass(GfRenderPass* renderPass)
{
	GF_ASSERT(!s_curState.m_curRenderPass, "Trying to begin a pass before end");
	s_curState.m_curRenderPass = renderPass;
	m_kPlatform.beginRenderPass(renderPass);
}

void GfCmdBuffer::endRenderPass()
{
	GF_ASSERT(s_curState.m_curRenderPass, "Trying to end an invalid pass");
	m_kPlatform.endRenderPass();
	s_curState.m_curRenderPass = nullptr;
}

void GfCmdBuffer::bindShaderPipe(const GfShaderVariant& shaderVariant)
{ 
	GfShaderPipeline* pipeline = shaderVariant.getPipeline();
	GfVariantHash variantHash = shaderVariant.getVariantHash();

	GF_ASSERT(pipeline->isCompute() || (!pipeline->isCompute() && s_curState.m_curVertexFormat), "Invalid active vertex format");
	GF_ASSERT(pipeline->isCompute() || (!pipeline->isCompute() && s_curState.m_curRenderPass), "Invalid active Render Pass");

	s_curState.m_curPipeline = pipeline;
	s_curState.m_curVariantHash = variantHash;
	m_kPlatform.bindShaderPipe(pipeline, variantHash, &s_curState.m_config, s_curState.m_curVertexFormat, s_curState.m_curRenderPass);
}

void GfCmdBuffer::submit(
	const GfWindow& kWindow,
	Bool bLast)
{
	m_kPlatform.submitRHI(*m_ctx, kWindow, m_queue, bLast);
	GF_ASSERT(m_cache, "Invalid cache");
	m_cache->returnToCache(this);
}

////////////////////////////////////////////////////////////////////////////////
// EOF
