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
	kOutFileHandle.m_ulFileSize = 0;
	kOutFileHandle.m_eType = eAccesType;
	kOutFileHandle.Plat().m_pHandle = CreateFileA(
		szFilename,
		g_pAccesModes[eAccesType],
		0, NULL,
		g_pCreationDisposition[eAccesType],
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (kOutFileHandle.Plat().m_pHandle == INVALID_HANDLE_VALUE)
	{
		GF_PRINT("Attempt to open file '%s' failed with error: %d", szFilename, GetLastError());
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void GfFile::CloseFile(GfFileHandle& kHandle)
{
	if (kHandle.Plat().m_pHandle != INVALID_HANDLE_VALUE) 
	{
		CloseHandle(kHandle.Plat().m_pHandle);
		kHandle.Plat().m_pHandle = INVALID_HANDLE_VALUE;
		kHandle.m_ulFileSize = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////

size_t GfFile::GetFileSize(GfFileHandle& kHandle)
{
	if (kHandle.Plat().m_pHandle != INVALID_HANDLE_VALUE) 
	{
		if (kHandle.m_ulFileSize == 0) 
		{
			LARGE_INTEGER ulSize;
			GetFileSizeEx(kHandle.Plat().m_pHandle, &ulSize);
			kHandle.m_ulFileSize = ulSize.QuadPart;
		}
		return kHandle.m_ulFileSize;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

u32 GfFile::ReadBytes(const GfFileHandle& kHandle, u32 uiToRead, void* pOutBuffer)
{
	if (kHandle.m_eType == EFileAccessMode::Read) 
	{
		DWORD uiOutRead(0);
		if (!ReadFile(kHandle.Plat().m_pHandle,
			pOutBuffer,
			uiToRead,
			&uiOutRead,
			NULL)) 
		{
			GF_PRINT("Attempt to read file data failed with error: %d", GetLastError());
		}
		return uiOutRead;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

u32 GfFile::WriteBytes(const GfFileHandle& kHandle, u32 uiToWrite, void* pBuffer)
{
	if (kHandle.m_eType == EFileAccessMode::Write) 
	{
		DWORD uiOutWrite(0);
		if (!WriteFile(
			kHandle.Plat().m_pHandle,
			pBuffer,
			uiToWrite,
			&uiOutWrite,
			NULL)) 
		{
			GF_PRINT("Attempt to read file data failed with error: %d", GetLastError());
		}
		return uiOutWrite;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfFileHandle)
	, m_pHandle(INVALID_HANDLE_VALUE)
{
}

////////////////////////////////////////////////////////////////////////////////
// EOF