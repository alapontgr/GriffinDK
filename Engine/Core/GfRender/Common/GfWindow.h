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

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfEntry/Common/GfEntry.h"
#include GF_SOLVE_PLATFORM_HEADER(GfWindow)


////////////////////////////////////////////////////////////////////////////////

struct GfWindowInitParams : public GfWindowInitParams_Platform
{
	GfEntryArgs m_kArguments;
	const char* m_szWindowName;
	u32         m_uiWidth;
	u32         m_uiHeight;
	bool        m_bFullScreen;
	bool        m_bVSync;
};

////////////////////////////////////////////////////////////////////////////////

struct GfEntryArgs;
class GfWindow
{
public:

	friend class GfWindow_Platform;

	GfWindow();

	void Init(GfWindowInitParams& kInitParams);

	// returns GF_FALSE when the user wants to close the application
	Bool Tick();

	void Shutdown();

	u32 GetWidth();

	u32 GetHeight();

private:

	GfWindow_Platform	m_kPlatform;
	const char*			m_szAppName; // Static string
	u32					m_uiWidth;
	u32					m_uiHeight;
	bool				m_bFullScreen;
	bool				m_bVSync;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfWindow::GetWidth()
{
	return m_uiWidth;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfWindow::GetHeight()
{
	return m_uiWidth;
}


////////////////////////////////////////////////////////////////////////////////
#endif // __GFWINDOW_H__