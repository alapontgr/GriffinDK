////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfRenderSync.cpp
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GraphicResources/GfRenderSync.h"

GF_DEFINE_BASE_CTOR(GfSemaphore)
{
}

void GfSemaphore::create(const GfRenderContext& ctx)
{
	m_kPlatform.create(ctx);
}

void GfSemaphore::destroy(const GfRenderContext& ctx)
{
	m_kPlatform.destroy(ctx);
}