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

#include "GfCore/Common/GfTypes.h"

#include <assert.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////

void GFPrintMsg(const char *fmt, ...);

void GFPrintError(const char* szFileName, u32 uiLine, const char *fmt, ...);

////////////////////////////////////////////////////////////////////////////////

#if (GF_LOGGING == GF_ON)

// Use for logging
#define GF_PRINT(FMT, ...) GFPrintMsg(FMT, __VA_ARGS__)

// Use it to validate an expression. If it is invalid it will show the filename/line/expresion and the message. Also, it will assert
#define GF_VERIFY(EXPR, MSG) \
  if(!(EXPR)) GFPrintError(__FILE__, __LINE__, "// Expr: '%s' failed.\n// Message: %s\n", #EXPR, MSG); abort();

#else

#define GF_PRINT(FMT, ...) GF_NOTHING
#define GF_VERIFY(EXPR) GF_NOTHING

#endif

////////////////////////////////////////////////////////////////////////////////

#if (GF_ASSERTIONS == GF_ON)

#define GF_ASSERT(EXPR, MSG) assert(EXPR && MSG)

#else

#define GF_ASSERT(EXPR, MSG) GF_NOTHING

#endif

#define  GF_ASSERT_ALWAYS(EXPR, MSG) assert(EXPR && MSG)

////////////////////////////////////////////////////////////////////////////////
#endif