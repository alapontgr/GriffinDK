#pragma once
//////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfRenderSync_Platform.h
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFRENDERSYNC_PLATFORM_H__
#define __GFRENDERSYNC_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfRender/GfGraphicsSDK.h"

class GfRenderContext;
class GfSemaphore;

class GfSemaphore_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfSemaphore);
public:

	friend class GfTexture;

	void create(const GfRenderContext& ctx);

	void destroy(const GfRenderContext& ctx);

	VkSemaphore getHandle() const { return m_semaphore; }

private:
	VkSemaphore m_semaphore;
};

#endif