////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/13
//	File: 	GfWindow.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFWINDOW_H__
#define __GFWINDOW_H__
////////////////////////////////////////////////////////////////////////////////
// Includes 

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfEntry/GfEntry.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GfWindow_Platform.h)

#include "Common/GfRender/GraphicResources/GfTexture.h"
#include "Common/GfRender/GraphicResources/GfRenderSync.h"

////////////////////////////////////////////////////////////////////////////////

struct GfWindowInitParams : public GfWindowInitParams_Platform
{
	GfEntryArgs m_kArguments;
	const char* m_szWindowName;
	u32         m_width;
	u32         m_height;
	bool        m_bFullScreen;
	bool        m_bVSync;
};

////////////////////////////////////////////////////////////////////////////////

struct GfEntryArgs;
class GfWindow
{
	GF_DECLARE_PLATFORM_INTERFACE(GfWindow);
public:

	GfWindow();

	void init(GfWindowInitParams& kInitParams, GfRenderContext& kCtx);

	// returns GF_FALSE when the user wants to close the application
	Bool tick();

	void shutdown();

	Bool beginFrame(const GfRenderContext& kCtx);

	void endFrame(const GfRenderContext& kCtx);

	u32 getWidth() const;

	u32 getHeight() const;

	const char* getWindowName();

	u32 getCurrentFrameIdx() const;

	u32 getNextFrameIdx() const;

	GfTexture2D* getBackBuffer(u32 uiIdx);

	GfTexture2D* getCurrBackBuffer();

	const GfSemaphore& getImageReadySemaphore() const;

	const GfSemaphore& getFinishedRenderingSemaphore() const;

private:

	void Flip();

	void OnResize();

	const char*			m_szAppName; // Static string
	u32					m_width;
	u32					m_height;
	bool				m_bFullScreen;
	bool				m_bVSync;

	// Used as a cursor for the multi buffering of the resources of the engine
	u32					m_uiCurrentFrameIdx;

	GfVector<GfTexture2D>	m_tSwapchainTextures;
	GfVector<GfSemaphore>	m_swapchainSemaphores;
	GfVector<GfSemaphore>	m_presentationSemaphore;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfWindow::getWidth() const
{
	return m_width;
}

GF_FORCEINLINE u32 GfWindow::getHeight() const
{
	return m_height;
}

GF_FORCEINLINE const char* GfWindow::getWindowName() 
{
	return m_szAppName;
}

GF_FORCEINLINE u32 GfWindow::getCurrentFrameIdx() const
{
	return m_uiCurrentFrameIdx;
}

GF_FORCEINLINE GfTexture2D* GfWindow::getBackBuffer(u32 uiIdx)
{
	return &m_tSwapchainTextures[uiIdx];
}

GF_FORCEINLINE GfTexture2D* GfWindow::getCurrBackBuffer()
{
	return &m_tSwapchainTextures[m_uiCurrentFrameIdx];
}

GF_FORCEINLINE const GfSemaphore& GfWindow::getImageReadySemaphore() const
{
	return m_swapchainSemaphores[getCurrentFrameIdx()];
}

GF_FORCEINLINE const GfSemaphore& GfWindow::getFinishedRenderingSemaphore() const
{
	return m_presentationSemaphore[getCurrentFrameIdx()];
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFWINDOW_H__