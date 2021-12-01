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

#include "Common/GfRender/GfWindow.h"
#include "Common/GfRender/GfRenderContext.h" 

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfWindow)
	, m_szAppName(nullptr)
	, m_width(0)
	, m_height(0)
	, m_bFullScreen(false)
	, m_bVSync(false)
	, m_uiCurrentFrameIdx(0)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow::init(GfWindowInitParams& kInitParams, GfRenderContext& kCtx)
{
	m_szAppName = kInitParams.m_szWindowName;
	m_width = kInitParams.m_width;
	m_height = kInitParams.m_height;
	m_bFullScreen = kInitParams.m_bFullScreen;
	m_bVSync = kInitParams.m_bVSync;
	m_kPlatform.initRHI(kInitParams);

	// Will pre-initialize the Render context only once (further calls will have no effect)
	kCtx.PreInit(this);
	m_kPlatform.CreateSurface(kCtx);
	// Only the first time this call will perform any actions
	kCtx.init(this);

	// Finally, after everything has been initialized, create the swapchain
	m_kPlatform.CreateSwapchain(kCtx);

	m_swapchainImageReady.resize(GfRenderConstants::ms_uiNBufferingCount);
	m_swapchainFinishRendering.resize(GfRenderConstants::ms_uiNBufferingCount);
	for (u32 i = 0; i < GfRenderConstants::ms_uiNBufferingCount; ++i)
	{
		m_swapchainImageReady[i].create(kCtx);
		m_swapchainFinishRendering[i].create(kCtx);
	}
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

u32 GfWindow::getNextFrameIdx() const
{
	return GfWrap(m_uiCurrentFrameIdx + 1, 0u, GfRenderConstants::ms_uiNBufferingCount);
}

void GfWindow::Flip()
{
	m_uiCurrentFrameIdx = getNextFrameIdx();
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow::OnResize()
{

}

////////////////////////////////////////////////////////////////////////////////
// EOF
