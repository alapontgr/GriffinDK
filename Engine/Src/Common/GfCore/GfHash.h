////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2020/01/10
//	File: 	GfHash.h
//
//	Copyright (c) 2020 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#pragma once 
#include "Common/GfCore/GfCoreMinimal.h"

////////////////////////////////////////////////////////////////////////////////

class GfHash 
{
public:

	//MurmurHash2

	// 64-bit version
	static u64 compute(const void* buffer, u64 size, u64 seed = 123);
	static u64 compute(const GfString& str, u64 seed = 123);
	// 32-bit version
	static u32 compute32(const void* buffer, u32 size, u32 seed = 123);
	static u32 compute32(const GfString& str, u32 seed = 123);
};

////////////////////////////////////////////////////////////////////////////////
