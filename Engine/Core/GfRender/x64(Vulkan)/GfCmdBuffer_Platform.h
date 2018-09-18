////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/16
//	File: 	GfCmdBuffer_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFCMDBUFFER_PLATFORM_H__
#define __GFCMDBUFFER_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfGraphicsSDK.h"
#include "GfRender/Common/GfRenderConstants.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

////////////////////////////////////////////////////////////////////////////////

struct GfCmdBufferSlot_Platform
{
	VkCommandBuffer m_pCmdBuffer	= nullptr;
	VkFence			m_pFence		= 0;
};

////////////////////////////////////////////////////////////////////////////////

class GfCmdBuffer_Platform
{
public:

	friend class GfCmdBufferFactory_Platform;

	GfCmdBuffer_Platform();

	void InitPlatform(
		VkCommandBuffer* pCmdBuffers,
		VkFence* pFences);

	void WaitForReadyPlatform(const GfRenderContext& kCtx);

private:

	// Perform Multi buffering of the command buffers to avoid waiting for the end of a previous execution
	GfCmdBufferSlot_Platform m_pEntries[GfRenderConstants::ms_uiNBufferingCount];
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFER_PLATFORM_H__