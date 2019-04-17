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

#include "GfInput/Common/GfInput.h"

#include <Windowsx.h>
#include <stdio.h>

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