////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/18
//	File: 	GfTexture2D.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GraphicResources/GfTexture2D.h"

////////////////////////////////////////////////////////////////////////////////

GfTexture2D::GfTexture2D()
	: GfTexture2D_Platform(*this)
	, GfGraphicsResourceBase(GetType())
{
}

////////////////////////////////////////////////////////////////////////////////
// EOF