////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/13
//	File: 	GFWINDOW.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfWindow.h"
#include "GfRender/Common/GfRenderContext.h" 

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfWindow)
	, m_szAppName(nullptr)
	, m_uiWidth(0)
	, m_uiHeight(0)
	, m_bFullScreen(false)
	, m_bVSync(false)
	, m_uiCurrentFrameIdx(0)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow::Init(GfWindowInitParams& kInitParams, GfRenderContext& kCtx)
{
	m_szAppName = kInitParams.m_szWindowName;
	m_uiWidth = kInitParams.m_uiWidth;
	m_uiHeight = kInitParams.m_uiHeight;
	m_bFullScreen = kInitParams.m_bFullScreen;
	m_bVSync = kInitParams.m_bVSync;
	m_kPlatform.InitRHI(kInitParams);

	// Will pre-initialize the Render context only once (further calls will have no effect)
	kCtx.PreInit(this);
	m_kPlatform.CreateSurface(kCtx);
	// Only the first time this call will perform any actions
	kCtx.Init(this);

	// Finally, after everything has been initialized, create the swapchain
	m_kPlatform.CreateSwapchain(kCtx);
	m_kPlatform.CreateSyncPrimitives(kCtx);
}

////////////////////////////////////////////////////////////////////////////////

Bool GfWindow::Tick()
{
	return m_kPlatform.TickRHI();
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow::Shutdown()
{
	m_kPlatform.ShutdownRHI();
}

////////////////////////////////////////////////////////////////////////////////

Bool GfWindow::BeginFrame(const GfRenderContext& kCtx)
{
	Bool bResult(Tick());
	m_kPlatform.BeginFrameRHI(kCtx);
	return bResult;
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow::EndFrame(const GfRenderContext& kCtx)
{
	m_kPlatform.EndFrameRHI(kCtx);
	Flip();
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow::Flip()
{
	m_uiCurrentFrameIdx = GfWrap(m_uiCurrentFrameIdx + 1, 0u, GfRenderConstants::ms_uiNBufferingCount);
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow::OnResize()
{

}

////////////////////////////////////////////////////////////////////////////////
// EOF
