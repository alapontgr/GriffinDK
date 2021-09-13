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
	EPrimitiveTopology::Type m_topology = EPrimitiveTopology::TriList;
	GfMultiSamplingState m_msaState;
};

class GfShaderPipeline 
{
	GF_DECLARE_PLATFORM_INTERFACE(GfShaderPipeline);
public:

	GfShaderPipeline();

	void setShaderBlob(GfUniquePtr<u8[]>&& shaderBlob, const u32 blobSize);

	const GfShaderDeserializer& getDeserializer() const { return m_shaderData; }

	bool isCompute() const { return m_shaderData.isCompute(); }

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