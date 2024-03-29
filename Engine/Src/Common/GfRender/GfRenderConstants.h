////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/18
//	File: 	GfRenderConstants.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFRENDERCONSTANTS_H__
#define __GFRENDERCONSTANTS_H__
////////////////////////////////////////////////////////////////////////////////
// Include 

#include "Common/GfCore/GfCoreMinimal.h"

////////////////////////////////////////////////////////////////////////////////

namespace GfRenderContextFamilies
{
	enum Type : u32
	{
		Graphics = 0,
		Compute,
		Transfer,
		Count,
		InvalidIdx = 0xffffffff
	};
}

////////////////////////////////////////////////////////////////////////////////

class GfRenderConstants 
{
public:

	// This constant defines the number of buffered buffers used by the application.
	static constexpr u32 ms_uiNBufferingCount = 3;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERCONSTANTS_H__