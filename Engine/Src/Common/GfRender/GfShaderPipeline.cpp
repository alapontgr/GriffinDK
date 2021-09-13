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