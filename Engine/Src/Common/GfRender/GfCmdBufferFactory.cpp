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

#include "Common/GfRender/GfCmdBufferFactory.h"

////////////////////////////////////////////////////////////////////////////////

GfCmdBufferFactory::GfCmdBufferFactory()
{
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBufferFactory::init(const GfRenderContext& kCtx, GfRenderContextFamilies::Type eQueueType)
{
	m_kPlatform.initRHI(kCtx, eQueueType);
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBufferFactory::CreateCmdBuffer(const GfRenderContext& kCtx, GfCmdBufferType::Type eType, GfCmdBuffer& kOuCmdBuffer)
{
	m_kPlatform.CreateCmdBufferRHI(kCtx, eType, kOuCmdBuffer);
}

////////////////////////////////////////////////////////////////////////////////
// EOF