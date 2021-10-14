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

#include "Common/GfCore/GfCoreMinimal.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GfRenderContext_Platform.h)

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext
{
	GF_DECLARE_PLATFORM_INTERFACE(GfRenderContext)
public:

	friend class GfWindow;

	GfRenderContext();

	void Shutdown();

	u32 GetFamilyIdx(GfRenderContextFamilies::Type eType) const;

	void tick();

	u32 getCurFrame() const { return m_curFrame; }

private:

	enum EFlags : u32 
	{
		PreInitialised	= 1<<0,
		Initialised		= 1<<1,
	};

	// Initialize with the first window ever created. Future windows will need a compatible surface
	void PreInit(GfWindow* pWindow);
	void init(GfWindow* pWindow);

	// Used Families
	u32							m_pAvailableFamilies[GfRenderContextFamilies::Count];
	GfBitMask<u32>				m_uiFlags;
	u32 m_curFrame;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfRenderContext::GetFamilyIdx(GfRenderContextFamilies::Type eType) const
{
	GF_ASSERT(eType >= 0 && eType < GfRenderContextFamilies::Count, "Invalid family");
	return m_pAvailableFamilies[eType];
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERCONTEXT_H__