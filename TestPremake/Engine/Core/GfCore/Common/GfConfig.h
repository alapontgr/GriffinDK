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
#endif