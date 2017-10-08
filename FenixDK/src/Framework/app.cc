#include "app.h"
#include "time.h"
//#include "input.h"

#include <Windows.h>
#include <chrono>
#include "Time.h"
#include "Rendering\RenderInterface.h"

namespace fdk
{
namespace Framework
{
  namespace TimeParams 
  {
    // Time features
    static std::chrono::high_resolution_clock::time_point lastFrame;
    static f64 updateDuration;
    static f64 renderDuration;
    static f64 totalTime;
    static f64 deltaTime;
  }

  static App* s_rCurrentApp;
	u32 App::s_width = 0;
	u32 App::s_height = 0;

	App::App() {}
	App::~App() {}

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
															LPARAM lParam);
	bool ProcessEvents(MSG* msg);

	void App::run(const char* title)
	{
    s_rCurrentApp = this;
		TimeParams::totalTime = 0.0;
    TimeParams::lastFrame = std::chrono::high_resolution_clock::now();

    // User pre init
		on_pre_init();
		
    init(title);
		
    // User start
    on_start();

		MSG msg;
		// wait for the next message in the queue, store the result in 'msg'
		while (true)
		{
			// Check to see if any messages are waiting in the queue
			if (ProcessEvents(&msg)) {
				// check to see if it's time to quit
				if (msg.message == WM_QUIT)
					break;
			}
			frame();
		}
		release();
	}

	void App::release()
	{
		on_release();
		// input::Release();
	}

	void App::frame()
	{
		auto t0 = std::chrono::high_resolution_clock::now();
    TimeParams::deltaTime = static_cast<f64>(
				std::chrono::duration_cast<std::chrono::microseconds>(t0 - TimeParams::lastFrame)
						.count());
    TimeParams::lastFrame = t0;
    TimeParams::totalTime += TimeParams::deltaTime;

		update();
		// Get update duration
		auto t1 = std::chrono::high_resolution_clock::now();
    TimeParams::updateDuration = static_cast<f64>(
				std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count());

		render();

		// Get render duration
		auto t2 = std::chrono::high_resolution_clock::now();
    TimeParams::renderDuration = static_cast<f64>(
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
	}

	void App::update()
	{
		// input::UpdateInput();
		on_update();
	}

	void App::render()
	{
		//m_context.begin_scene(m_clearColor);
		on_render();
		//m_context.end_scene();
	}

	void App::init(const char* title)
	{
		// this struct holds information for the window class
		WNDCLASSEX wc;

		// clear out the window class for use
		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		// fill in the struct with the needed information
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = GetModuleHandle(0);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		// wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.lpszClassName = "SampleClass";
		// register the window class
		RegisterClassEx(&wc);

		s32 width = GetSystemMetrics(SM_CXSCREEN);
		s32 height = GetSystemMetrics(SM_CYSCREEN);

		DEVMODE dev_screen_settings;
		s32 pos_x = 0, pos_y = 0;
		if (m_fullscreen) {
			s_width = width;
			s_height = height;
			memset(&dev_screen_settings, 0, sizeof(dev_screen_settings));
			dev_screen_settings.dmSize = sizeof(dev_screen_settings);
			dev_screen_settings.dmPelsWidth = (unsigned long)width;
			dev_screen_settings.dmPelsHeight = (unsigned long)height;
			dev_screen_settings.dmBitsPerPel = 32;
			dev_screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			ChangeDisplaySettings(&dev_screen_settings, CDS_FULLSCREEN);
		}
		else
		{
			width = s_width;
			height = s_height;
			pos_x = GetSystemMetrics(SM_CXSCREEN) >> 2;
			pos_y = GetSystemMetrics(SM_CYSCREEN) >> 2;
		}

		RECT wr = {0, 0, width, height};									 // set the size, but not the position
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); // adjust the size

		// create the window and use the result as the handle
		auto hwnd = CreateWindowEx(NULL,
														"SampleClass",			 // name of the window class
														title,							 // title of the window
														WS_OVERLAPPEDWINDOW, // window style
														pos_x,							 // x-position of the window
														pos_y,							 // y-position of the window
														wr.right - wr.left,	// width of the window
														wr.bottom - wr.top,	// height of the window
														NULL,								 // we have no parent window, NULL
														NULL,								 // we aren't using menus, NULL
                            GetModuleHandle(0),					 // application handle
														NULL);							 // used with multiple windows, NULL

		// display the window on the screen
		ShowWindow(hwnd, SW_SHOWNORMAL);

		// init graphic context
		//m_context.init();
		// Init Input context
		// input::Init(hInstance, hwnd, width_, height_);
    Rendering::RenderInterface::init();
  }

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

	// this is the main message handler for the program
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
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

	f64 Time::delta_time_secs() { return TimeParams::deltaTime * 0.000001; }
	f64 Time::delta_time_mili() { return TimeParams::deltaTime * 0.001; }
	f64 Time::delta_time_micro() { return TimeParams::deltaTime; }
	f64 Time::total_time_secs() { return TimeParams::totalTime * 0.000001; }
	f64 Time::total_time_mili() { return TimeParams::totalTime * 0.001; }
	f64 Time::total_time_micro() { return TimeParams::totalTime; }
	f64 Time::update_time() { return TimeParams::updateDuration; }
	f64 Time::render_time() { return TimeParams::renderDuration; }
}
}