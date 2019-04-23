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

#include "GfRender/Common/GfRenderContext.h"
#include "GfRender/Common/GfWindow.h"
#include "GfRender/Common/GfRenderConstants.h"
#include "GfCore/Common/GfMaths.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfRenderContext)
	, m_uiFlags(0)
{
	for (u32 i=0; i<GfRenderContextFamilies::Count; ++i) 
	{
		// Give an invalid value as the default value
		m_pAvailableFamilies[i] = GfRenderContextFamilies::InvalidIdx;
	}
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

void GfRenderContext::Init(GfWindow* pWindow)
{
	if (!m_uiFlags.IsEnable(EFlags::Initialised)) 
	{
		m_uiFlags.Enable(EFlags::Initialised);
		m_kPlatform.InitRHI(pWindow);
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext::Shutdown()
{
	GF_ASSERT_ALWAYS("To Implement");
}

////////////////////////////////////////////////////////////////////////////////
// EOF
