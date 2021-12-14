////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/15
//	File: 	GFRENDERCONTEXT.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GfRenderContext.h"
#include "Common/GfRender/GfWindow.h"
#include "Common/GfRender/GfRenderConstants.h"
#include "Common/GfCore/GfMaths.h"
#include "Common/GfRender/GraphicResources/GfGraphicResources.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfRenderContext)
	, m_uiFlags(0)
	, m_curFrame(0)
{
	for (u32 i=0; i<GfRenderContextFamilies::Count; ++i) 
	{
		// Give an invalid value as the default value
		m_pAvailableFamilies[i] = GfRenderContextFamilies::InvalidIdx;
	}
}

void GfRenderContext::tick() 
{
	m_curFrame++;
	m_kPlatform.tick();

	ResourceFactory<GfBuffer>::tick(*this);
	ResourceFactory<GfTexture>::tick(*this);
	ResourceFactory<GfSampler>::tick(*this);
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext::PreInit(GfWindow* pWindow)
{
	if (!m_uiFlags.IsEnable(EFlags::PreInitialised))
	{
		m_uiFlags.Enable(EFlags::PreInitialised);
		m_kPlatform.PreInitRHI(pWindow);
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext::init(GfWindow* pWindow)
{
	if (!m_uiFlags.IsEnable(EFlags::Initialised)) 
	{
		m_uiFlags.Enable(EFlags::Initialised);
		m_kPlatform.initRHI(pWindow);
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext::Shutdown()
{
	ResourceFactory<GfBuffer>::shutdown(*this);
	ResourceFactory<GfTexture>::shutdown(*this);
	ResourceFactory<GfSampler>::shutdown(*this);

	GF_ASSERT_ALWAYS("To Implement");
}

////////////////////////////////////////////////////////////////////////////////
// EOF
