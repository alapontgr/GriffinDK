////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/09
//	File: 	GfConfig.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFCONFIG_H__
#define __GFCONFIG_H__
////////////////////////////////////////////////////////////////////////////////

#define GF_IDENT(ARG) ARG
#define GF_STRINGIFY_2(ARG) #ARG
#define GF_STRINGIFY(ARG) GF_STRINGIFY_2(ARG)
#define GF_CONCAT(A, B) A##B

#define GF_SOLVE_PLATFORM_FILE(PLATFORM, FILE, EXT) GF_STRINGIFY(../GF_IDENT(PLATFORM)/GF_IDENT(FILE)_Platform.GF_IDENT(EXT))

// Use this macro to include the platform "header" version of a Common file
#define GF_SOLVE_PLATFORM_HEADER(FILE) GF_SOLVE_PLATFORM_FILE(_PLATFORM_DIR, FILE, h)
// Use this macro to include the platform "inline file" version of a Common file
#define GF_SOLVE_PLATFORM_INLINE(FILE) GF_SOLVE_PLATFORM_FILE(_PLATFORM_DIR, FILE, inl)

// Force inline expression
#define GF_FORCEINLINE __forceinline

// Align ass
#define GF_ALIGNAS(ALIGN) alignas(ALIGN)

////////////////////////////////////////////////////////////////////////////////

// Used as enabled/disabled flags for macros
#define GF_OFF  0
#define GF_ON   1

#define GF_NOTHING ((void)0)

// Useful for breakpoints
#define GF_NOPE GF_NOTHING

#ifdef _DEBUG

#define GF_LOGGING    GF_ON
#define GF_ASSERTIONS GF_ON

#elif defined(_DEBUGOPT)

#define GF_LOGGING    GF_ON
#define GF_ASSERTIONS GF_ON

#elif defined(_RELEASE)

#define GF_LOGGING    GF_ON
#define GF_ASSERTIONS GF_OFF

#elif defined(_RELEASEFINAL)

#define GF_LOGGING    GF_ON
#define GF_ASSERTIONS GF_OFF

#endif

////////////////////////////////////////////////////////////////////////////////

#define GF_ENGINE_NAME "GriffinEngine"
#define GF_ENGINE_VERSION 0

////////////////////////////////////////////////////////////////////////////////
#endif