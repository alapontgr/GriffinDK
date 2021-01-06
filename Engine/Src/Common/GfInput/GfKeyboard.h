////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/18
//	File: 	GfKeyboard.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFKEYBOARD_H__
#define __GFKEYBOARD_H__
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfCore/GfStl.h"

////////////////////////////////////////////////////////////////////////////////

namespace GfKey 
{
	enum Type : u8
	{
		// Chars
		N_0,
		N_1,
		N_2,
		N_3,
		N_4,
		N_5,
		N_6,
		N_7,
		N_8,
		N_9,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		SemiColumn,	// ';:'
		Plus,		//	'=+'
		Comma,		//	',<'
		Minus,		//	'-_'
		Period,		//	'.>'
		Oem2,		//	'/?'
		Oem3,		//	'~'
		Oem4,		//	'[{'
		Oem5,		//	'\|'
		Oem6,		//	']}'
		Oem7,		//	''"'
		Oem102,		// '\\'
		Space,		// ' '
		// Non chars
		Oem8,		//	''
		Back = 0,
		Tab,
		Clear,
		Enter,
		Alt,
		Pause,
		Cap,
		Esc,
		PageUp,
		PageDown,
		End,
		Home,
		Left,
		Up,
		Right,
		Down,
		Select,
		Print,
		Exec,
		PrintScr,
		Ins,
		Del,
		Help,
		LWin,
		RWin,
		NumPad_0,
		NumPad_1,
		NumPad_2,
		NumPad_3,
		NumPad_4,
		NumPad_5,
		NumPad_6,
		NumPad_7,
		NumPad_8,
		NumPad_9,
		Mul,
		Add,
		Sep,
		Sub,
		Decimal,
		Div,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		NumLock,
		ScrollLock,
		LShift,
		RShift,
		LCtrl,
		RCtrl,
		////////////////////////////////////////////////////////////////////////////////
		COUNT,
		LAST_CHAR = Space,
		INVALID = 0xff
	};
}

////////////////////////////////////////////////////////////////////////////////

class GfKeyboard 
{
public:

	friend class GfInput_Platform;

	GfKeyboard();

	void Update();

	char KeyToChar(GfKey::Type eKey, bool bShifted = false);

private:

	struct KeyEvent
	{
		union 
		{
			u8 m_bDown : 1;
			u8 m_uiKey : 7;
		};
	};

	void SetKeyDown(GfKey::Type eKey);
	void SetKeyUp(GfKey::Type eKey);

	GfQueue<KeyEvent> m_tEventsToProcess;
	u8 m_pKeyDownMap[GfKey::COUNT];
	u8 m_pKeysPressedMap[GfKey::COUNT];
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFKEYBOARD_H__