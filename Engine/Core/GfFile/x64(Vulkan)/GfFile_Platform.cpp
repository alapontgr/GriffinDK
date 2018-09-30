////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/30
//	File: 	GfFile_Platform.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfFile/Common/GfFile.h"

////////////////////////////////////////////////////////////////////////////////
// Must be the smae order as EFileAccessMode::Type

const DWORD g_pAccesModes[] = 
{
	GENERIC_READ,
	GENERIC_WRITE,
};

////////////////////////////////////////////////////////////////////////////////
// Must be the smae order as EFileAccessMode::Type

const DWORD g_pCreationDisposition[] =
{
	OPEN_EXISTING,	// Opens the file only if it exists
	CREATE_ALWAYS,	// Creates always a file. If it already exists it will overwrite it
};

////////////////////////////////////////////////////////////////////////////////
// File operations

bool GfFile::OpenFile(const char* szFilename, EFileAccessMode::Type eAccesType, GfFileHandle& kOutFileHandle)
{
	kOutFileHandle.m_kBase.m_ulFileSize = 0;
	kOutFileHandle.m_pHandle = CreateFileA(
		szFilename,
		g_pAccesModes[eAccesType],
		0, NULL,
		g_pCreationDisposition[eAccesType],
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (kOutFileHandle.m_pHandle == INVALID_HANDLE_VALUE) 
	{
		GF_PRINT("Attempt to open file '%s' failed with error: %d", szFilename, GetLastError());
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void GfFile::CloseFile(GfFileHandle& kHandle)
{
	if (kHandle.m_pHandle != INVALID_HANDLE_VALUE) 
	{
		CloseHandle(kHandle.m_pHandle);
		kHandle.m_pHandle = INVALID_HANDLE_VALUE;
		kHandle.m_ulFileSize = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////

size_t GfFile::GetFileSize(GfFileHandle& kHandle)
{
	if (kHandle.m_pHandle != INVALID_HANDLE_VALUE) 
	{
		if (kHandle.m_ulFileSize == 0) 
		{
			LARGE_INTEGER ulSize;
			GetFileSizeEx(kHandle.m_pHandle, &ulSize);
			kHandle.m_ulFileSize = ulSize.QuadPart;
		}
		return kHandle.m_ulFileSize;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

u32 GfFile::ReadBytes(const GfFileHandle& kHandle, u32 uiToRead, void* pOutBuffer)
{
	// TODO
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

u32 GfFile::WriteBytes(const GfFileHandle& kHandle, u32 uiToWrite, void* pBuffer)
{
	// TODO
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

GfFileHandle_Platform::GfFileHandle_Platform(GfFileHandle& kBase)
	: m_kBase(kBase)
	, m_pHandle(INVALID_HANDLE_VALUE)
{
}

////////////////////////////////////////////////////////////////////////////////
// EOF