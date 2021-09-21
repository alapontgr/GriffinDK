////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfShaderPipeline.h
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfRender/GfShaderPipeline.h"
#include "GfShaderPipeline.h"

////////////////////////////////////////////////////////////////////////////////

GfVertexDeclaration::GfVertexDeclaration()
{}

void GfVertexDeclaration::init(const AttributeDesc * attribArray, const u32 attribCount, const VertexBufferBinding * vertexBufferBindings, u32 vertexBufferBindCount)
{
	m_attributesDescs.insert(m_attributesDescs.begin(), attribArray, attribArray+attribCount);
	m_vertexBuffersDescs.insert(m_vertexBuffersDescs.begin(), vertexBufferBindings, vertexBufferBindings+vertexBufferBindCount);
	// Build hash based on data from both vectors. Note second time it uses itself as seed hash
	m_hash = GfHash::compute(m_attributesDescs.data(), m_attributesDescs.size() * sizeof(AttributeDesc));
	m_hash = GfHash::compute(m_vertexBuffersDescs.data(), m_vertexBuffersDescs.size() * sizeof(VertexBufferBinding), m_hash);
}

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfShaderPipeline)
{
}

void GfShaderPipeline::setShaderBlob(GfUniquePtr<u8[]>&& shaderBlob, const u32 blobSize)
{
	GF_ASSERT_ALWAYS("TODO");
}

GfShaderVariant::GfShaderVariant(GfShaderPipeline* shaderPipeline)
	: m_pipeline(shaderPipeline)
	, m_variantHash(0)
{}

void GfShaderVariant::enableKeyWord(const GfString& mutator) 
{
	GF_ASSERT_ALWAYS("TODO");
}

void GfShaderVariant::disableKeyWord(const GfString& mutator)
{
	GF_ASSERT_ALWAYS("TODO");
}