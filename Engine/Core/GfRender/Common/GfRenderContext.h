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

class GfRenderContext
{
	GF_DECLARE_PLATFORM_INTERFACE(GfRenderContext)
public:

	GfRenderContext();

	// Initialize with the first window ever created. Future windows will need a compatible surface
	void PreInit(GfWindow* pWindow);
	void Init(GfWindow* pWindow);

	void Shutdown();

	u32 GetFamilyIdx(GfRenderContextFamilies::Type eType) const;

private:

	enum EFlags : u32 
	{
		PreInitialised	= 1<<0,
		Initialised		= 1<<1,
	};

	// Used Families
	u32							m_pAvailableFamilies[GfRenderContextFamilies::Count];
	GfBitMask<u32>				m_uiFlags;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfRenderContext::GetFamilyIdx(GfRenderContextFamilies::Type eType) const
{
	GF_ASSERT(eType >= 0 && eType < GfRenderContextFamilies::Count, "Invalid family");
	return m_pAvailableFamilies[eType];
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFRENDERCONTEXT_H__