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

#include "Common/GfCore/GfCoreMinimal.h"
#include GF_SOLVE_PLATFORM_PATH(GfFile/GfFile_Platform.h)

////////////////////////////////////////////////////////////////////////////////

namespace EFileAccessMode 
{
	enum Type : u32 
	{
		Read = 0,
		Write,
		Invalid = 0xffffffff,
	};
}

////////////////////////////////////////////////////////////////////////////////

class GfFileHandle
{
	GF_DECLARE_PLATFORM_INTERFACE(GfFileHandle);
public:

	friend class GfFile;

	GfFileHandle();

	size_t GetFileSize() const { return m_ulFileSize; }

private:

	size_t					m_ulFileSize;
	EFileAccessMode::Type	m_eType;
};

////////////////////////////////////////////////////////////////////////////////

class GfFile
{
public:

	static bool OpenFile(const char* szFilename, EFileAccessMode::Type eAccesType, GfFileHandle& kOutFileHandle);

	static void CloseFile(GfFileHandle& kHandle);

	static bool DoesFileExist(const char* filePath);

	static size_t GetFileSize(GfFileHandle& kHandle);

	static u32 ReadBytes(const GfFileHandle& kHandle, u32 uiToRead, void* pOutBuffer);

	static u32 WriteBytes(const GfFileHandle& kHandle, u32 uiToWrite, const void* pBuffer);

private:
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFFILE_H__