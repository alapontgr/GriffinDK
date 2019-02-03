////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/26
//	File: 	GfMaterial_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMATERIAL_PLATFORM_H__
#define __GFMATERIAL_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfGraphicsSDK.h"
#include "GfRender/Common/GfMaterialShared.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

////////////////////////////////////////////////////////////////////////////////

class GfMaterialTemplate_Platform 
{
public:

	friend class GfMaterialTemplate;

	GfMaterialTemplate_Platform(GfMaterialTemplate& kBase);

protected:

	void DestroyRHI(const GfRenderContext& kCtx);

	void CreateRHI(const GfRenderContext& kCtx);

private:

	void CreateLayout(const GfRenderContext& kCtx);

	void CreatePipeline(const GfRenderContext& kCtx);

	GfMaterialTemplate&		m_kBase;
	VkPipelineLayout		m_pLayout;
	VkPipeline				m_pPipeline;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIAL_PLATFORM_H__