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

	Bool BeginFrame();

	void EndFrame();

	u32 GetFamilyIdx(GfRencerContextFamilies::Type eType) const;

	GfWindow* GetWindow() const;

private:

	void Flip();

	void OnResize();

	GfWindow*					m_pWindow;

	// Used Families
	u32							m_pAvailableFamilies[GfRencerContextFamilies::Count];

	// Used as a cursor for the multi buffering of the resources of the engine
	u32							m_uiCurrentFrameIdx;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfRenderContext::GetCurrentFrameIdx() const
{
	return m_uiCurrentFrameIdx;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfRenderContext::GetFamilyIdx(GfRencerContextFamilies::Type eType) const
{
	GF_ASSERT(eType >= 0 && eType < GfRencerContextFamilies::Count, "Invalid family");
	return m_pAvailableFamilies[eType];
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE GfWindow* GfRenderContext::GetWindow() const
{
	return m_pWindow;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERCONTEXT_H__