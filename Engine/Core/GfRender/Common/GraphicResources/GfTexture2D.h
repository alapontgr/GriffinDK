////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/18
//	File: 	GfTexture2D.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFTEXTURE2D_H__
#define __GFTEXTURE2D_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfRender/Common/GraphicResources/GfGraphicResourceBase.h"
#include GF_SOLVE_PLATFORM_HEADER_EXT(GfRender, GraphicResources/GfTexture2D)

////////////////////////////////////////////////////////////////////////////////

class GfTexture2D : public GfTexture2D_Platform, public GfGraphicsResourceBase
{
public:

	friend class GfTexture2D_Platform;

	static EParamaterSlotType::Type GetType() { return EParamaterSlotType::SampledImage; }

	GfTexture2D();

private:

};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTURE2D_H__