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

GfRenderContext::GfRenderContext()
	: GfRenderContext_Platform(*this)
	, m_kPlatform(*this)
	, m_pWindow(nullptr)
	, m_uiCurrentFrameIdx(0)
{
	for (u32 i=0; i<GfRencerContextFamilies::Count; ++i) 
	{
		// Give an invalid value as the default value
		m_pAvailableFamilies[i] = 0xffffffff;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext::Init(GfWindow* pWindow)
{
	m_pWindow = pWindow;
	InitInternal();
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext::Shutdown()
{

}

////////////////////////////////////////////////////////////////////////////////

Bool GfRenderContext::BeginFrame()
{
	Bool bResult(m_pWindow->Tick());
	m_kPlatform.BeginFramePlatform();
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext::EndFrame()
{
	m_kPlatform.EndFramePlatform();
	Flip();
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext::Flip()
{
	m_uiCurrentFrameIdx = GfWrap(m_uiCurrentFrameIdx + 1, 0u, GfRenderConstants::ms_uiNBufferingCount);
}

////////////////////////////////////////////////////////////////////////////////

void GfRenderContext::OnResize()
{

}

////////////////////////////////////////////////////////////////////////////////
// EOF
