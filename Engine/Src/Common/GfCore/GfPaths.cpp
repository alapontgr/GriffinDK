////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2021/01/13
//	File: 	GFDEBUG.cpp
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfCore/GfPaths.h"
#include "Common/GfCore/GfCommandLine.h"
#include "Common/GfCore/GfHash.h"

////////////////////////////////////////////////////////////////////////////////

GfString GfPaths::ms_workingDir = "./";

static GfString s_workingDirArg = "--workDir";

void GfPaths::init()
{
	GfString workDir = GfCommandLine::getArg(GfHash::compute(s_workingDirArg.data(), s_workingDirArg.size()));
	ms_workingDir = "./";
	if (workDir.size() > 0) 
	{
		if (workDir[workDir.size()-1] != '/' && workDir[workDir.size()-1] != '\\') 
		{
			workDir += "/";
		}
		ms_workingDir = workDir;
		std::replace( ms_workingDir.begin(), ms_workingDir.end(), '\\', '/');
	}
}

GfString GfPaths::getAssetPath(const GfString& relPath) 
{
	static const char* s_prefix = "./";
	size_t pos = relPath.find(s_prefix);
	GfString path = relPath;
	if (pos != GfString::npos) 
	{
		path = relPath.substr(pos + 2);
	}
	GfString absPath = ms_workingDir + path;
	std::replace( absPath.begin(), absPath.end(), '\\', '/');
	return absPath;
}

GfString GfPaths::getAssetPath(const char* relPath)
{
	return getAssetPath(GfString(relPath));
}

////////////////////////////////////////////////////////////////////////////////