////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/12
//	File: 	GFENTRY.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfEntry/GfEntry.h"

////////////////////////////////////////////////////////////////////////////////

s32 WINAPI WinMain(
	HINSTANCE pInstance,
	HINSTANCE pPrevInstance,
	LPSTR szCmdLine,
	s32 siCmdShow)
{
	GfEntryArgs kParams;
	kParams.m_szCmdLine = szCmdLine;
	_GfEntry_(kParams);
}

s32 main(s32 argc, char* argv[])
{
	GfEntryArgs kParams;
	kParams.m_szCmdLine = "";
	for (s32 i = 0; i < argc; ++i) 
	{
		kParams.m_szCmdLine += argv[i] + GfString((i < argc - 1) ? " " : "");
	}
	_GfEntry_(kParams);
}

////////////////////////////////////////////////////////////////////////////////
// EOF