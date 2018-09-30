////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/30
//	File: 	GfFile.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFFILE_H__
#define __GFFILE_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfCore/Common/GfCoreMinimal.h"
#include GF_SOLVE_PLATFORM_HEADER(GfFile)

////////////////////////////////////////////////////////////////////////////////

namespace EFileAccessMode 
{
	enum Type : u32 
	{
		Read = 0,
		Write,
	};
}

////////////////////////////////////////////////////////////////////////////////

class GfFileHandle : public GfFileHandle_Platform
{
public:

	friend class GfFileHandle_Platform;
	friend class GfFile;

	GfFileHandle();

private:

	size_t m_ulFileSize;
};

////////////////////////////////////////////////////////////////////////////////

class GfFile : public GfFile_Platform
{
public:

	static bool OpenFile(const char* szFilename, EFileAccessMode::Type eAccesType, GfFileHandle& kOutFileHandle);

	static void CloseFile(GfFileHandle& kHandle);

	static size_t GetFileSize(GfFileHandle& kHandle);

	static u32 ReadBytes(const GfFileHandle& kHandle, u32 uiToRead, void* pOutBuffer);

	static u32 WriteBytes(const GfFileHandle& kHandle, u32 uiToWrite, void* pBuffer);

private:
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFFILE_H__