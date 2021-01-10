#pragma once 
////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2020/01/10
//	File: 	GfCommandLine.h
//
//	Copyright (c) 2020 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfCoreMinimal.h"

////////////////////////////////////////////////////////////////////////////////

class GfCommandLine 
{
public:
	static void init(const GfString& args);

	static GfString getArg(const u64 argHash);

private:

	static GfString ms_commandLineArgs;
	static GfUMap<u64, GfString> ms_arguments;
};