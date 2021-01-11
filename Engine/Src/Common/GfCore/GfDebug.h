////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/09
//	File: 	GfDebug.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFDEBUG_H__
#define __GFDEBUG_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfCore/GfTypes.h"

#include <assert.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////

enum MsgType 
{
	Normal,
	Info,
	Warning,
	Error,
	COUNT
};

void GFPrintMsg(MsgType type, const char *fmt, ...);

void GFPrintError(const char* szFileName, u32 uiLine, const char *fmt, ...);

////////////////////////////////////////////////////////////////////////////////

#if (GF_LOGGING == GF_ON)

// Use for logging
#define GF_PRINT(FMT, ...) GFPrintMsg(MsgType::Normal, FMT, __VA_ARGS__)
#define GF_INFO(FMT, ...) GFPrintMsg(MsgType::Info, FMT, __VA_ARGS__)
#define GF_WARNING(FMT, ...) GFPrintMsg(MsgType::Warning, FMT, __VA_ARGS__)
#define GF_ERROR(FMT, ...) GFPrintMsg(MsgType::Error, FMT, __VA_ARGS__); abort();

// Use it to validate an expression. If it is invalid it will show the filename/line/expresion and the message. Also, it will assert
#define GF_VERIFY(EXPR, MSG) \
  if(!(EXPR)) GFPrintError(__FILE__, __LINE__, "// Expr: '%s' failed.\n// Message: %s\n", #EXPR, MSG); abort();

#else

#define GF_PRINT(FMT, ...) GF_NOTHING
#define GF_INFO(FMT, ...) GF_NOTHING
#define GF_WARNING(FMT, ...) GF_NOTHING
#define GF_ERROR(FMT, ...) GF_NOTHING
#define GF_VERIFY(EXPR, MSG) GF_NOTHING

#endif

////////////////////////////////////////////////////////////////////////////////

#if (GF_ASSERTIONS == GF_ON)

#define GF_ASSERT(EXPR, MSG) assert(EXPR && MSG)

#else

#define GF_ASSERT(EXPR, MSG) GF_NOTHING

#endif

// Causes the application to assert every time this macro is reached
#define  GF_ASSERT_ALWAYS(MSG) assert(0 && MSG)

////////////////////////////////////////////////////////////////////////////////
#endif