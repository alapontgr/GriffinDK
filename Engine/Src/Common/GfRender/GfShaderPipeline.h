#pragma once
////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfShaderPipeline.h
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFSHADERPIPELINE_H__
#define __GFSHADERPIPELINE_H__

////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfRender/GfRenderCommon.h"
#include "Common/GfShaderCompiler/GfShaderCache.h"

#include GF_SOLVE_GFX_API_PATH(GfRender/GfShaderPipeline_Platform.h)

////////////////////////////////////////////////////////////////////////////////

// Configure how to render
struct GfShaderPipeConfig
{
	GfBlendState m_blendState;
	GfRasterState m_rasterState;
	PrimitiveTopology::Type m_topology = PrimitiveTopology::TriList;
	GfMultiSamplingState m_msaState;
	GfDepthState m_depthState;
};

class GfVertexDeclaration 
{
public:

	struct AttributeDesc 
	{
		u32	m_uiOffset : 12; // Offset within the struct
		u32 m_location : 4; // Shader binding location
		u32 m_vertexBufferIdx : 4; // Index of the vertex buffer the data belongs to
		AttributeFormat::Type	m_eType : 12;	// Type of the attribute
	};

	struct VertexBufferBinding 
	{
		u16 m_bufferIdx : 4;				// Vertex buffer bind slot
		u16 m_stride : 11;					// Distance in bytes of two consecutive elements
		VertexInputRate::Type m_rate : 1;	// Vertex: 0, Instance: 1
	};

	GfVertexDeclaration();

	void init(const AttributeDesc* attribArray, const u32 attribCount, const VertexBufferBinding* vertexBufferBindings, u32 vertexBufferBindCount);

	const GfVector<AttributeDesc>& getAttributes() const { return m_attributesDescs; }

	const GfVector<VertexBufferBinding>& getVertexBuffers() const { return m_vertexBuffersDescs; }

	u64 getHash() const { return m_hash; }

	u32 getAttribCount() const { return static_cast<u32>(m_attributesDescs.size()); }

	u32 getVertexBufferBindCount() const { return static_cast<u32>(m_vertexBuffersDescs.size()); }

private:

	GfVector<AttributeDesc> m_attributesDescs;
	GfVector<VertexBufferBinding> m_vertexBuffersDescs;
	u64 m_hash;
};

class GfShaderPipeline 
{
	GF_DECLARE_PLATFORM_INTERFACE(GfShaderPipeline);
public:

	GfShaderPipeline();

	void setShaderBlob(GfUniquePtr<u8[]>&& shaderBlob, const u32 blobSize);

	const GfShaderDeserializer& getDeserializer() const { return m_shaderData; }

	bool isCompute() const { return m_shaderData.isCompute(); }

	u32 getUsedStages() const { return m_shaderData.getUsedStages(); }

private:

	GfShaderDeserializer m_shaderData;
};

class GfShaderVariant 
{
public:
	GfShaderVariant(GfShaderPipeline* shaderPipeline);
	
	void enableKeyWord(const GfString& mutator);

	void disableKeyWord(const GfString& mutator);
	
	GfVariantHash getVariantHash() const { return m_variantHash; }

	GfShaderPipeline* getPipeline() const { return m_pipeline; }

private:
	GfShaderPipeline* m_pipeline;
	GfVariantHash m_variantHash;
};

#endif