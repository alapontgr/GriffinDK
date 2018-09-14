////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/12
//	File: 	GfEntry_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFENTRY_PLATFORM_H__
#define __GFENTRY_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include <windows.h>

////////////////////////////////////////////////////////////////////////////////

struct GfEntryArgs_Platform \
{
	HINSTANCE m_pInstance;
	HINSTANCE m_pPrevInstance;
	LPSTR m_szCmdLine;
	s32 m_siCmdShow;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFENTRY_PLATFORM_H__