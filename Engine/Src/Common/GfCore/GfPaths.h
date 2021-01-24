////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2021/01/13
//	File: 	GfPaths.h
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#pragma once 
#include "Common/GfCore/GfCoreMinimal.h"

////////////////////////////////////////////////////////////////////////////////

class GfPaths 
{
public:

	static void init();

	// Convert relative path to absolute path in the working dir. Return <workingDir>/<relPath>
	static GfString getAssetPath(const GfString& relPath);
	static GfString getAssetPath(const char* relPath);

	static GfString getWorkingDir() 
	{
		return ms_workingDir;
	}

private:

	// Commandline: "--workDir <path>"
	static GfString ms_workingDir;
};

////////////////////////////////////////////////////////////////////////////////