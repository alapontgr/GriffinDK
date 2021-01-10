////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2020/01/10
//	File: 	GfCommandLine.cpp
//
//	Copyright (c) 2020 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfCommandLine.h"
#include "Common/GfCore/GfHash.h"
#include <sstream>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////

GfString GfCommandLine::ms_commandLineArgs = "";
GfUMap<u64, GfString> GfCommandLine::ms_arguments;

void GfCommandLine::init(const GfString& args) 
{
	ms_commandLineArgs = args;

	// Tokenize
	ms_arguments.clear();
	if (args.size() > 0) 
	{
		GfString token;
		GfString payload = "";
		std::istringstream tokenStream(ms_commandLineArgs);
		u64 hash = 0;
		while (std::getline(tokenStream, token, ' '))
		{
			if (token[0] == '-') 
			{
				if (hash != 0 && payload.size() != 0) 
				{
					ms_arguments[hash] = payload;
					payload = "";
				}
				hash = GfHash::compute(token);
			} else
			{
				payload += " " + token;
			}
		}

		if (hash != 0 && payload.size() != 0) 
		{
			ms_arguments[hash] = payload;
		}
	}
}

GfString GfCommandLine::getArg(const u64 argHash) 
{
	auto it = ms_arguments.find(argHash);
	if (it != ms_arguments.end()) 
	{
		return it->second;
	}
	return "";
}