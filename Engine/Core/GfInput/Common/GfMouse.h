////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/16
//	File: 	GfMouse.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMOUSE_H__
#define __GFMOUSE_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfCore/Common/GfMaths.h"

////////////////////////////////////////////////////////////////////////////////

class GfMouse 
{
public:

	friend class GfInput_Platform;

	enum MouseButtons : u8
	{
		Left = 0,
		Rigth,
		Middle
	};

	GfMouse();

	void Update();

	bool IsButtonDown(MouseButtons eButton) const;
	bool IsButtonPressed(MouseButtons eButton) const;
	f32 GetWheelDelta() const;
	v2 GetMousePos() const;

private:

	void SetButtonDown(MouseButtons eButton);
	void SetButtonUp(MouseButtons eButton);
	void SetMousePos(const v2& vPos);
	void SetWheelDelta(f32 fVal);

	v2 m_vMousePosition;
	u8 m_uiButtonsDown;		// Active for one frame
	u8 m_uiButtonsPressed;	// Active while the button is being held
	f32 m_pWheelDelta[2];	// Perform buffering
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfMouse::SetButtonDown(MouseButtons eButton)
{
	m_uiButtonsDown |= (1 << eButton);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfMouse::SetButtonUp(MouseButtons eButton)
{
	u8 uiMask(~(1 << eButton));
	m_uiButtonsPressed &= uiMask;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMOUSE_H__