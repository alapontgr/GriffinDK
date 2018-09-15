////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/13
//	File: 	GfWindow_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFWINDOW_PLATFORM_H__
#define __GFWINDOW_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfRender/Common/GfWindow.h"

////////////////////////////////////////////////////////////////////////////////

struct GfWindowInitParams_Platform
{

};

////////////////////////////////////////////////////////////////////////////////

struct GfWindowInitParams;

class GfWindow_Platform
{
public:

	friend class GfWindow;

	GfWindow_Platform(GfWindow& kBase);

	void Init(GfWindowInitParams& kInitParams);

	Bool Tick();

	void Shutdown();

	HWND GetHwnd();

	HINSTANCE GetInstance();

private:

	GfWindow& m_kBase;

	HWND		m_pHwnd;
	HINSTANCE	m_pInstance;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE HWND GfWindow_Platform::GetHwnd() 
{
	return m_pHwnd;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE HINSTANCE GfWindow_Platform::GetInstance()
{
	return m_pInstance;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFWINDOW_PLATFORM_H__