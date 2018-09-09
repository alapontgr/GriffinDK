////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/09
//	File: 	GfCore.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFCORE_H__
#define __GFCORE_H__
////////////////////////////////////////////////////////////////////////////////

#define GF_STRINGIFY(ARG) #ARG
#define GF_CONCAT(A, B) A##B

#define GF_SOLVE_PLATFORM_FILE(PLAT_DIR, Class, EXT) "../" GF_STRINGIFY(PLAT_DIR) "/" GF_STRINGIFY(Class) "_Platform" GF_STRINGIFY(EXT)

// Use this macro to include the platform "header" version of a Common file
#define GF_SOLVE_PLATFORM_HEADER(FILENAME) GF_SOLVE_PLATFORM_FILE(_PLATFORM_DIR, FILENAME, .h)
// Use this macro to include the platform "inline file" version of a Common file
#define GF_SOLVE_PLATFORM_INLINE(FILENAME) GF_SOLVE_PLATFORM_FILE(_PLATFORM_DIR, FILENAME, .inl)

////////////////////////////////////////////////////////////////////////////////

#include "GfCore/Common/GfTypes.h"

#include "GfCore/Common/GfConfig.h"

#include "GfCore/Common/GfDebug.h"


////////////////////////////////////////////////////////////////////////////////
#endif