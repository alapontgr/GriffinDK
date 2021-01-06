////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/16
//	File: 	GfCmdBufferFactory.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFCMDBUFFERFACTORY_H__
#define __GFCMDBUFFERFACTORY_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfCore/GfCoreMinimal.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GfCmdBufferFactory_Platform.h)

#include "Common/GfRender/GfCmdBuffer.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

////////////////////////////////////////////////////////////////////////////////

class GfCmdBufferFactory
{
	GF_DECLARE_PLATFORM_INTERFACE(GfCmdBufferFactory);
public:

	GfCmdBufferFactory();

	void Init(const GfRenderContext& kCtx, GfRenderContextFamilies::Type eQueueType);

	void CreateCmdBuffer(const GfRenderContext& kCtx, GfCmdBufferType::Type eType, GfCmdBuffer& kOuCmdBuffer);

	void Shutdown();

private:

};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFCMDBUFFERFACTORY_H__