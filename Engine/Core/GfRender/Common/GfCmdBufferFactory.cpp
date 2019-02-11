////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/16
//	File: 	GfCmdBufferFactory.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfCmdBufferFactory.h"

////////////////////////////////////////////////////////////////////////////////

GfCmdBufferFactory::GfCmdBufferFactory()
{
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBufferFactory::Init(const GfRenderContext& kCtx, GfRenderContextFamilies::Type eQueueType)
{
	InitRHI(kCtx, eQueueType);
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBufferFactory::CreateCmdBuffer(const GfRenderContext& kCtx, GfCmdBufferType::Type eType, GfCmdBuffer& kOuCmdBuffer)
{
	CreateCmdBufferRHI(kCtx, eType, kOuCmdBuffer);
}

////////////////////////////////////////////////////////////////////////////////
// EOF