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

#include "GfCore/Common/GfDebug.h"
#include <stdio.h>
#include <stdarg.h>

////////////////////////////////////////////////////////////////////////////////

static void GFPrintMsg(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int rc = vfprintf(stdout, fmt, args);
	va_end(args);
}

////////////////////////////////////////////////////////////////////////////////

void GFPrintError(const char* szFileName, u32 uiLine, const char *fmt, ...)
{
	va_list args;
	fprintf(stderr, "////////////////////////////////////////////////////////////////////////////////\n");
	fprintf(stderr, "// File: %s.\n// Line: %d \n", szFileName, uiLine);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "////////////////////////////////////////////////////////////////////////////////\n");
}

////////////////////////////////////////////////////////////////////////////////
// EOF