////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/09
//	File: 	GFDEBUG.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfCore/GfDebug.h"
#include <stdio.h>
#include <stdarg.h>

////////////////////////////////////////////////////////////////////////////////

static const char* s_colorCodes[u32(MsgType::COUNT)] = 
{
	"\033[0;37m",
	"\033[1;34mINFO - ",
	"\033[1;33mWARNING - ",
	"\033[1;31mERROR - ",
};

////////////////////////////////////////////////////////////////////////////////

void GFPrintMsg(MsgType type, const char *fmt, ...)
{
	va_list args;
	fprintf(stderr, s_colorCodes[static_cast<u32>(type)]);
	va_start(args, fmt);
	int rc = vfprintf(stdout, fmt, args);
	va_end(args);
	fprintf(stderr, "\033[0m\n");
}

////////////////////////////////////////////////////////////////////////////////

void GFPrintError(const char* szFileName, u32 uiLine, const char *fmt, ...)
{
	va_list args;
	fprintf(stderr, "\033[1;31m");
	fprintf(stderr, "////////////////////////////////////////////////////////////////////////////////\n");
	fprintf(stderr, "// File: %s.\n// Line: %d \n", szFileName, uiLine);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "////////////////////////////////////////////////////////////////////////////////\n");
	fprintf(stderr, "\033[0m");
}

////////////////////////////////////////////////////////////////////////////////
// EOF