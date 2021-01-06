////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/16
//	File: 	GfInput_Platform.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfInput/GfInput.h"

#include <Windowsx.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// See: https://docs.microsoft.com/en-us/windows/desktop/inputdev/virtual-key-codes

static constexpr GfKey::Type g_pWinKeyToGfKey[] = 
{
	GfKey::INVALID,		// The keys start at 0x1
	GfKey::INVALID,		// VK_LBUTTON
	GfKey::INVALID,		// VK_RBUTTON
	GfKey::INVALID,		// VK_CANCEL
	GfKey::INVALID,		// VK_MBUTTON
	GfKey::INVALID,		// VK_XBUTTON1
	GfKey::INVALID,		// VK_XBUTTON2
	GfKey::INVALID,
	GfKey::Back,		// VK_BACK
	GfKey::Tab,			// VK_TAB
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::Clear,		// VK_CLEAR
	GfKey::Enter,		// VK_RETURN
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,		// VK_SHIFT
	GfKey::INVALID,		// VK_CONTROL
	GfKey::Alt,			// VK_MENU
	GfKey::Pause,		// VK_PAUSE
	GfKey::Cap,			// VK_CAPITAL
	GfKey::INVALID,		// VK_KANA, VK_HANGUEL, VK_HANGUL
	GfKey::INVALID,
	GfKey::INVALID,		// VK_JUNJA
	GfKey::INVALID,		// VK_FINAL
	GfKey::INVALID,		// VK_HANJA, VK_KANJI
	GfKey::INVALID,
	GfKey::Esc,			// VK_ESCAPE
	GfKey::INVALID,		// VK_CONVERT
	GfKey::INVALID,		// VK_NONCONVERT
	GfKey::INVALID,		// VK_ACCEPT
	GfKey::INVALID,		// VK_MODECHANGE
	GfKey::Space,		// VK_SPACE
	GfKey::PageUp,		// VK_PRIOR
	GfKey::PageDown,	// VK_NEXT
	GfKey::End,			// VK_END
	GfKey::Home,		// VK_HOME
	GfKey::Left,		// VK_LEFT
	GfKey::Up,			// VK_UP
	GfKey::Right,		// VK_RIGHT
	GfKey::Down,		// VK_DOWN
	GfKey::Select,		// VK_SELECT
	GfKey::Print,		// VK_PRINT
	GfKey::Exec,		// VK_EXECUTE
	GfKey::PrintScr,	// VK_SNAPSHOT
	GfKey::Ins,			// VK_INSERT
	GfKey::Del,			// VK_DELETE
	GfKey::Help,		// VK_HELP
	GfKey::N_0,			// 0 key
	GfKey::N_1,			// 1 key
	GfKey::N_2,			// 2 key
	GfKey::N_3,			// 3 key
	GfKey::N_4,			// 4 key
	GfKey::N_5,			// 5 key
	GfKey::N_6,			// 6 key
	GfKey::N_7,			// 7 key
	GfKey::N_8,			// 8 key
	GfKey::N_9,			// 9 key
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::A,			// A
	GfKey::B,			// B
	GfKey::C,			// C
	GfKey::D,			// D
	GfKey::E,			// E
	GfKey::F,			// F
	GfKey::G,			// G
	GfKey::H,			// H
	GfKey::I,			// I
	GfKey::J,			// J
	GfKey::K,			// K
	GfKey::L,			// L
	GfKey::M,			// M
	GfKey::N,			// N
	GfKey::O,			// O
	GfKey::P,			// P
	GfKey::Q,			// Q
	GfKey::R,			// R
	GfKey::S,			// S
	GfKey::T,			// T
	GfKey::U,			// U
	GfKey::V,			// V
	GfKey::W,			// W
	GfKey::X,			// X
	GfKey::Y,			// Y
	GfKey::Z,			// Z
	GfKey::LWin,		// VK_LWIN
	GfKey::RWin,		// VK_RWIN
	GfKey::INVALID,		// VK_APPS
	GfKey::INVALID,
	GfKey::INVALID,		// VK_SLEEP
	GfKey::NumPad_0,	// VK_NUMPAD0
	GfKey::NumPad_1,	// VK_NUMPAD1
	GfKey::NumPad_2,	// VK_NUMPAD2
	GfKey::NumPad_3,	// VK_NUMPAD3
	GfKey::NumPad_4,	// VK_NUMPAD4
	GfKey::NumPad_5,	// VK_NUMPAD5
	GfKey::NumPad_6,	// VK_NUMPAD6
	GfKey::NumPad_7,	// VK_NUMPAD7
	GfKey::NumPad_8,	// VK_NUMPAD8
	GfKey::NumPad_9,	// VK_NUMPAD9
	GfKey::Mul,			// VK_MULTIPLY
	GfKey::Add,			// VK_ADD
	GfKey::Sep,			// VK_SEPARATOR
	GfKey::Sub,			// VK_SUBTRACT
	GfKey::Decimal,		// VK_DECIMAL
	GfKey::Div,			// VK_DIVIDE
	GfKey::F1,			// VK_F1
	GfKey::F2,			// VK_F2
	GfKey::F3,			// VK_F3
	GfKey::F4,			// VK_F4
	GfKey::F5,			// VK_F5
	GfKey::F6,			// VK_F6
	GfKey::F7,			// VK_F7
	GfKey::F8,			// VK_F8
	GfKey::F9,			// VK_F9
	GfKey::F10,			// VK_F10
	GfKey::F11,			// VK_F11
	GfKey::F12,			// VK_F12
	GfKey::INVALID,		// VK_F13
	GfKey::INVALID,		// VK_F14
	GfKey::INVALID,		// VK_F15
	GfKey::INVALID,		// VK_F16
	GfKey::INVALID,		// VK_F17
	GfKey::INVALID,		// VK_F18
	GfKey::INVALID,		// VK_F19
	GfKey::INVALID,		// VK_F20
	GfKey::INVALID,		// VK_F21
	GfKey::INVALID,		// VK_F22
	GfKey::INVALID,		// VK_F23
	GfKey::INVALID,		// VK_F24
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::NumLock,		// VK_NUMLOCK
	GfKey::ScrollLock,	// VK_SCROLL
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::LShift,		// VK_LSHIFT
	GfKey::RShift,		// VK_RSHIFT
	GfKey::LCtrl,		// VK_LCONTROL
	GfKey::RCtrl,		// VK_RCONTROL
	GfKey::INVALID,		// VK_LMENU
	GfKey::INVALID,		// VK_RMENU
	GfKey::INVALID,		// VK_BROWSER_BACK
	GfKey::INVALID,		// VK_BROWSER_FORWARD
	GfKey::INVALID,		// VK_BROWSER_REFRESH
	GfKey::INVALID,		// VK_BROWSER_STOP
	GfKey::INVALID,		// VK_BROWSER_SEARCH
	GfKey::INVALID,		// VK_BROWSER_FAVORITES
	GfKey::INVALID,		// VK_BROWSER_HOME
	GfKey::INVALID,		// VK_VOLUME_MUTE
	GfKey::INVALID,		// VK_VOLUME_DOWN
	GfKey::INVALID,		// VK_VOLUME_UP
	GfKey::INVALID,		// VK_MEDIA_NEXT_TRACK
	GfKey::INVALID,		// VK_MEDIA_PREV_TRACK
	GfKey::INVALID,		// VK_MEDIA_STOP
	GfKey::INVALID,		// VK_MEDIA_PLAY_PAUSE
	GfKey::INVALID,		// VK_LAUNCH_MAIL
	GfKey::INVALID,		// VK_LAUNCH_MEDIA_SELECT
	GfKey::INVALID,		// VK_LAUNCH_APP1
	GfKey::INVALID,		// VK_LAUNCH_APP2
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::SemiColumn,	// VK_OEM_1
	GfKey::Plus,		// VK_OEM_PLUS
	GfKey::Comma,		// VK_OEM_COMMA
	GfKey::Minus,		// VK_OEM_MINUS
	GfKey::Period,		// VK_OEM_PERIOD
	GfKey::Oem2,		// VK_OEM_2
	GfKey::Oem3,		// VK_OEM_3
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::Oem4,		// VK_OEM_4
	GfKey::Oem5,		// VK_OEM_5
	GfKey::Oem6,		// VK_OEM_6
	GfKey::Oem7,		// VK_OEM_7
	GfKey::Oem8,		// VK_OEM_8
	GfKey::INVALID,
	GfKey::INVALID,
	GfKey::Oem102,		// VK_OEM_102
};
static constexpr u64 g_ulLastKeyInTable = VK_OEM_102;

////////////////////////////////////////////////////////////////////////////////

void GfInput_Platform::HandleInput(u32 uiMsg, u64 ulParam, s64 slParam)
{
	// Handle mouse events
	if (uiMsg >= WM_MOUSEFIRST && uiMsg <= WM_MOUSELAST)
	{
		if (GfMouse* pMouse = GfInput::GetMousePtr())
		{
			switch (uiMsg)
			{
			case WM_MOUSEMOVE:
			{
				s16 siXPos = GET_X_LPARAM(slParam);
				s16 siYPos = GET_Y_LPARAM(slParam);
				v2 vPos((f32)siXPos, (f32)siYPos);
				pMouse->SetMousePos(vPos);
				break;
			}
			case WM_LBUTTONDOWN:
				pMouse->SetButtonDown(GfMouse::Left);
				break;
			case WM_LBUTTONUP:
				pMouse->SetButtonDown(GfMouse::Left);
				break;
			case WM_RBUTTONDOWN:
				pMouse->SetButtonDown(GfMouse::Rigth);
				break;
			case WM_RBUTTONUP:
				pMouse->SetButtonDown(GfMouse::Rigth);
				break;
			case WM_MBUTTONDOWN:
				pMouse->SetButtonDown(GfMouse::Middle);
				break;
			case WM_MBUTTONUP:
				pMouse->SetButtonDown(GfMouse::Middle);
				break;
			case WM_MOUSEWHEEL: 
			{
				s16 siDelta = GET_WHEEL_DELTA_WPARAM(ulParam);
				f32 fDelta((f32)siDelta / (f32)WHEEL_DELTA);
				pMouse->SetWheelDelta(fDelta);
				break;
			}
			default:
				break;
			}
		}
	}

	// Handle keyboard
	GfKeyboard* pKeyboard(GfInput::GetKeyboardPtr());
	if (uiMsg == WM_KEYDOWN) 
	{
		u64 ulKey(ulParam);
		if (ulKey <= g_ulLastKeyInTable) 
		{
			GfKey::Type eKey(g_pWinKeyToGfKey[ulKey]);
			pKeyboard->SetKeyDown(eKey);
		}
	}
	if (uiMsg == WM_KEYUP)
	{
		u64 ulKey(ulParam);
		if (ulKey <= g_ulLastKeyInTable)
		{
			GfKey::Type eKey(g_pWinKeyToGfKey[ulKey]);
			pKeyboard->SetKeyUp(eKey);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

bool GfInput_Platform::IsMousePresent()
{
	return GetSystemMetrics(SM_MOUSEPRESENT) == TRUE;
}

////////////////////////////////////////////////////////////////////////////////

bool GfInput_Platform::IsKeyBoardPresent()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// EOF