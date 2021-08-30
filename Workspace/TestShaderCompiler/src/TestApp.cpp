////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/03/30
//	File: 	TestApp.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "./src/TestApp.h"
#include "Common/GfInput/GfInput.h"
#include "Common/GfShaderCompiler/GfShaderCompiler.h"

////////////////////////////////////////////////////////////////////////////////

static GfUniquePtr<char[]> LoadFileSrc(const char* szFilepath, u32& uiOutFileSize)
{
	GfFileHandle kFile;
	GfFile::OpenFile(szFilepath, EFileAccessMode::Read, kFile);
	GfFile::GetFileSize(kFile);
	uiOutFileSize = static_cast<u32>(kFile.GetFileSize());
	if (uiOutFileSize > 0) 
	{
		GfUniquePtr<char[]> pSrc(new char[uiOutFileSize+1]);
		u32 uiRead = GfFile::ReadBytes(kFile, uiOutFileSize, pSrc.get());
		GfFile::CloseFile(kFile);
		GF_ASSERT(uiRead == uiOutFileSize, "Invalid size read");
		pSrc[uiOutFileSize] = 0;
		return pSrc;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

s32 TestApp::Run(const GfEntryArgs& kEntryParams)
{
	GfCommandLine::init(kEntryParams.m_szCmdLine);

	GfString shaderCache = GfCommandLine::getArg(GfHash::compute("-sc"), ".");
	GfString testShader = GfCommandLine::getArg(GfHash::compute("-f"));

	if (shaderCache.size() == 0) 
	{
		return 1;
	}

	if (testShader.size() == 0) 
	{
		return 1;
	}

	u32 srcSize = 0;
	GfUniquePtr<char[]> src = LoadFileSrc(testShader.c_str(), srcSize);
	GfString shaderSrc(src.get());
	std::replace( shaderSrc.begin(), shaderSrc.end(), '\\', '/');
	if (shaderSrc.find("\r\n") != GfString::npos) 
	{
		shaderSrc.erase(std::remove(shaderSrc.begin(), shaderSrc.end(), '\r'), shaderSrc.end());
	}

	GfString errors;
	GfShaderCacheFile filter;
	filter.init(shaderCache);
	GfShaderCompiler compiler;
	compiler.init("", "");
	compiler.compileShader(testShader.c_str(), shaderSrc, filter, errors);

	if (errors.size() > 0) 
	{
		GF_ERROR("%s", errors.c_str());
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// EOF