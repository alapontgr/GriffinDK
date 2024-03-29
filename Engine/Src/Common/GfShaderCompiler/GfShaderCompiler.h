#pragma once
////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfShaderCompiler.h
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFSHADERCOMPILER_H__
#define __GFSHADERCOMPILER_H__

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfRender/GfRenderCommon.h"
#include "Common/GfShaderCompiler/GfShaderCache.h"

////////////////////////////////////////////////////////////////////////////////

class GfShaderCompiler 
{
public:

	GfShaderCompiler();
	~GfShaderCompiler();

	void init(const GfString& globalShaderPath, const GfString& baseShaderPath);

	bool compileShader(const GfString& filename, const GfString& src, GfShaderCache& filter, GfString& outErrors);

private:

	bool parse(const GfString& filename, const GfString& src, GfShaderSerializer& outSerializer, GfString& outSrc, GfString& outErrors);

	GfVector<GfVariantHash> uberize(const GfShaderSerializer& serializer);

	bool compileVariant(const GfString& parsedSrc,
		const GfShaderSerializer& serializer,
		const GfVariantHash hash,
		GfShaderSerializer::ShaderVariant* variant, 
		GfVector<GfShaderSerializer::ShaderBytecode>* bytecodeCache,
		GfVector<u32>* bytecodeSizes,
		GfUMap<u64, s32>* hashToIdx,
		GfString& outErrors,
		GfString& outWarnings);

	bool reflectVariant(GfShaderSerializer& serializer,
		GfShaderSerializer::ShaderVariant* variant, 
		GfString& outErrors);

	GfString m_baseShaderDirPath;
	GfString m_engineShaderDirPath; 
};

#endif