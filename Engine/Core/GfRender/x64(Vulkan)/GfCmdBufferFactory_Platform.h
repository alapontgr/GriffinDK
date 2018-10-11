////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/16
//	File: 	GfCmdBufferFactory_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFCMDBUFFERFACTORY_PLATFORM_H__
#define __GFCMDBUFFERFACTORY_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfGraphicsSDK.h"
#include "GfRender/Common/GfRenderConstants.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;
class GfCmdBuffer;

////////////////////////////////////////////////////////////////////////////////

class GfCmdBufferFactory_Platform
{
public:

	friend class GfCmdBufferFactory;

	GfCmdBufferFactory_Platform();

private:

	void InitRHI(const GfRenderContext& kCtx, GfRencerContextFamilies::Type eQueueType);

	void CreateCmdBufferRHI(const GfRenderContext& kCtx, u32 uiType, GfCmdBuffer& kOuCmdBuffer);

	VkCommandPool m_pPool;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFERFACTORY_PLATFORM_H__