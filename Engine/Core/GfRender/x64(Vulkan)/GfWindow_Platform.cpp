////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/13
//	File: 	GFWINDOW_PLATFORM.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfWindow.h"

////////////////////////////////////////////////////////////////////////////////
// Functions

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
		// This message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	}
	break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////

bool ProcessEvents(MSG* msg)
{
	// Check to see if any messages are waiting in the queue
	if (PeekMessage(msg, NULL, 0, 0, PM_REMOVE)) {
		// translate keystroke messages into the right format
		TranslateMessage(msg);
		// send the message to the WindowProc function
		DispatchMessage(msg);
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// Header impl

GfWindow_Platform::GfWindow_Platform(GfWindow& kBase)
	: m_kBase(kBase)
	, m_pHwnd(nullptr)
	, m_pInstance(nullptr)
{}

////////////////////////////////////////////////////////////////////////////////

void GfWindow_Platform::InitPlatform(GfWindowInitParams& kInitParams)
{
	// Assign the HANDLE
	m_pInstance = GetModuleHandle(NULL);

	// this struct holds information for the window class
	WNDCLASSEX kWc;
	// clear out the window class for use
	ZeroMemory(&kWc, sizeof(WNDCLASSEX));

#ifdef DEAD
	kWc.cbSize = sizeof(WNDCLASSEX);
	kWc.style = CS_HREDRAW | CS_VREDRAW;
	kWc.lpfnWndProc = WndProc;
	kWc.cbClsExtra = 0;
	kWc.cbWndExtra = 0;
	kWc.hInstance = info.connection;  // hInstance
	kWc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	kWc.hCursor = LoadCursor(NULL, IDC_ARROW);
	kWc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	kWc.lpszMenuName = NULL;
	kWc.lpszClassName = info.name;
	kWc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
#endif // DEAD

	// fill in the struct with the needed information
	kWc.cbSize = sizeof(WNDCLASSEX);
	kWc.style = CS_HREDRAW | CS_VREDRAW;
	kWc.lpfnWndProc = WindowProc;
	kWc.hInstance = m_pInstance;
	kWc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	kWc.hCursor = LoadCursor(NULL, IDC_ARROW);
	kWc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	kWc.lpszMenuName = NULL;
	kWc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	// Window name
	wchar_t wszAppname[4096];
	MultiByteToWideChar(CP_ACP, 0, kInitParams.m_szWindowName, -1, wszAppname, 4096);
	kWc.lpszClassName = wszAppname;

	// register the window class
	RegisterClassEx(&kWc);

	DEVMODE dev_screen_settings;
	s32 siPosX = 0, siPosY = 0;
	if (kInitParams.m_bFullScreen) {
		m_kBase.m_uiWidth = GetSystemMetrics(SM_CXSCREEN);
		m_kBase.m_uiHeight = GetSystemMetrics(SM_CYSCREEN);
		memset(&dev_screen_settings, 0, sizeof(dev_screen_settings));
		dev_screen_settings.dmSize = sizeof(dev_screen_settings);
		dev_screen_settings.dmPelsWidth = (unsigned long)m_kBase.m_uiWidth;
		dev_screen_settings.dmPelsHeight = (unsigned long)m_kBase.m_uiHeight;
		dev_screen_settings.dmBitsPerPel = 32;
		dev_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		ChangeDisplaySettings(&dev_screen_settings, CDS_FULLSCREEN);
	}
	else
	{
		siPosX = GetSystemMetrics(SM_CXSCREEN) >> 2;
		siPosY = GetSystemMetrics(SM_CYSCREEN) >> 2;
	}

	// set the size, but not the position
	RECT wr = { 0, 0, (s64)m_kBase.m_uiWidth, (s64)m_kBase.m_uiHeight };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); // adjust the size

	// create the window and use the result as the handle
	m_pHwnd = CreateWindowEx(NULL,
		wszAppname,										// name of the window class
		wszAppname,										// title of the window
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU,	// window style
		siPosX,											// x-position of the window
		siPosY,											// y-position of the window
		wr.right - wr.left,								// width of the window
		wr.bottom - wr.top,								// height of the window
		NULL,			   								// we have no parent window, NULL
		NULL,			   								// we aren't using menus, NULL
		GetModuleHandle(NULL),							// application handle
		NULL);											// used with multiple windows, NULL
	
	GF_ASSERT(m_pHwnd, "Failed to create the window");

	// display the window on the screen
	ShowWindow(m_pHwnd, SW_SHOWNORMAL);
}

////////////////////////////////////////////////////////////////////////////////

Bool GfWindow_Platform::TickPlatform()
{
	MSG msg;
	// Check to see if any messages are waiting in the queue
	if (ProcessEvents(&msg))
	{
		// check to see if it's time to quit
		if (msg.message == WM_QUIT) 
		{
			return GF_FALSE;
		}
	}
	return GF_TRUE;
}

////////////////////////////////////////////////////////////////////////////////

void GfWindow_Platform::ShutdownPlatform()
{
	CloseWindow(m_pHwnd);
	m_pHwnd = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// EOF