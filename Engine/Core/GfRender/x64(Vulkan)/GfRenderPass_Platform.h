////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/17
//	File: 	GfRenderPass_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFRENDERPASS_PLATFORM_H__
#define __GFRENDERPASS_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfGraphicsSDK.h"
#include "GfRender/Common/GfRenderConstants.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

////////////////////////////////////////////////////////////////////////////////

class GfRenderPass_Platform 
{
public:

	friend class GfRenderPass;

	GfRenderPass_Platform(GfRenderPass& kBase);

protected:

	void CompileRHI(const GfRenderContext& kCtx);

private:

	void RecreateFramebufferRHI(const GfRenderContext& kCtx);

	GfRenderPass& m_kBase;

public:
	// TODO: Revise this encapsulation
	VkFramebuffer	m_pFramebuffers[GfRenderConstants::ms_uiNBufferingCount];
	VkRenderPass	m_pRenderPass;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERPASS_PLATFORM_H__