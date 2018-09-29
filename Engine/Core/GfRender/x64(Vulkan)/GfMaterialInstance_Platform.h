////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/26
//	File: 	GfMaterialInstance_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMATERIALINSTANCE_PLATFORM_H__
#define __GFMATERIALINSTANCE_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfGraphicsSDK.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

////////////////////////////////////////////////////////////////////////////////

class GfMaterialInstance_Platform 
{
public:

	friend class GfMaterialinstance;

	GfMaterialInstance_Platform(GfMaterialinstance& kBase);

private:

	void DestroyPlatform(const GfRenderContext& kCtx);

	void CreatePlatform(const GfRenderContext& kCtx);

	void CreateLayout(const GfRenderContext& kCtx);

	void CreatePipeline(const GfRenderContext& kCtx);

	GfMaterialinstance&		m_kBase;
	VkPipelineLayout		m_pLayout;
	VkPipeline				m_pPipeline;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIALINSTANCE_PLATFORM_H__