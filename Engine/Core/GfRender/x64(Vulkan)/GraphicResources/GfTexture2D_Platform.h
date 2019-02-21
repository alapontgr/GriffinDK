////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/18
//	File: 	GfTexture2D_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFTEXTURE2D_PLATFORM_H__
#define __GFTEXTURE2D_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfRender/Common/GfGraphicsSDK.h"

////////////////////////////////////////////////////////////////////////////////

class GfTexture2D_Platform 
{
public:

	friend class GfTexture2D;

	GfTexture2D_Platform(GfTexture2D& kBase);

private:

	GfTexture2D& m_kbase;

	VkImage m_pImage;
	VkImageView m_pImageView;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTURE2D_PLATFORM_H__