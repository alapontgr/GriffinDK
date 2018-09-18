////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/15
//	File: 	GfRenderContext.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFRENDERCONTEXT_H__
#define __GFRENDERCONTEXT_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfCore/Common/GfCoreMinimal.h"
#include GF_SOLVE_PLATFORM_HEADER(GfRenderContext)

////////////////////////////////////////////////////////////////////////////////

class GfWindow;

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext : public GfRenderContext_Platform
{
public:

	friend class GfRenderContext_Platform;

	GfRenderContext();

	void Init(GfWindow* pWindow);

	void Shutdown();

	u32 GetCurrentFrameIdx() const;

	void BeginFrame();

	void EndFrame();

private:

	void Flip();

	GfRenderContext_Platform	m_kPlatform;
	GfWindow*					m_pWindow;

	// Used as a cursor for the multi buffering of the resources of the engine
	u32							m_uiCurrentFrameIdx;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfRenderContext::GetCurrentFrameIdx() const
{
	return m_uiCurrentFrameIdx;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERCONTEXT_H__