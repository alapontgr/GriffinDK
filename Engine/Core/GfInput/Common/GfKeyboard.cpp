////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/18
//	File: 	GfKeyboard.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfInput/Common/GfKeyboard.h"

////////////////////////////////////////////////////////////////////////////////

struct GfKeyToCharEntry 
{
	char m_siChar;			// Char for the key
	char m_siShiftedChar;	// Its version when Shift is presed
};

////////////////////////////////////////////////////////////////////////////////

static GfKeyToCharEntry g_pKeyToChar[] = 
{
	{ '0', ')' },
	{ '1', '!' },
	{ '2', '@' },
	{ '3', '#' },
	{ '4', '$' },
	{ '5', '%' },
	{ '6', '^' },
	{ '7', '&' },
	{ '8', '*' },
	{ '9', '(' },
	{ 'a', 'A' },
	{ 'b', 'B' },
	{ 'c', 'C' },
	{ 'd', 'D' },
	{ 'e', 'E' },
	{ 'f', 'F' },
	{ 'g', 'G' },
	{ 'h', 'H' },
	{ 'i', 'I' },
	{ 'j', 'J' },
	{ 'k', 'K' },
	{ 'l', 'L' },
	{ 'm', 'M' },
	{ 'n', 'N' },
	{ 'o', 'O' },
	{ 'p', 'P' },
	{ 'q', 'Q' },
	{ 'r', 'R' },
	{ 's', 'S' },
	{ 't', 'T' },
	{ 'u', 'U' },
	{ 'v', 'V' },
	{ 'w', 'W' },
	{ 'x', 'X' },
	{ 'y', 'Y' },
	{ 'z', 'Z' },
	{ ';', ':' },
	{ '=', '+' },
	{ ',', '<' },
	{ '-', '_' },
	{ '.', '>' },
	{ '/', '?' },
	{ '~', '~' },
	{ '[', '{' },
	{ '\\', '|' },
	{ ']', '}' },
	{ '\'', '"' },
	{ '\\', '|' },
	{ ' ', ' ' },
};

////////////////////////////////////////////////////////////////////////////////

GfKeyboard::GfKeyboard()
{
	memset(m_pKeyDownMap, 0, sizeof(u8) * GfKey::COUNT);
}


////////////////////////////////////////////////////////////////////////////////

void GfKeyboard::Update()
{
	// 1. Update pressed keys with the keys down of the previous frame
	for (u8 i=0; i<GfKey::COUNT; ++i) 
	{
		m_pKeysPressedMap[i] |= m_pKeyDownMap[i]; // The key will be down for one frame, after that it becomes pressed until it is clear by the KeyUp event
	}

	// 2. Clear the buttons down so they have a lifetime of one frame
	memset(m_pKeyDownMap, 0, sizeof(u8) * GfKey::COUNT);

	// 3. Process pending events
	while (m_tEventsToProcess.size()) 
	{
		KeyEvent kEvt = m_tEventsToProcess.front();
		m_tEventsToProcess.pop();
		// It should be impossible to press and release the same button the same frame
		m_pKeyDownMap[kEvt.m_uiKey] = kEvt.m_bDown;
		// If there is a KeyUp event, the key stops being pressed immediately
		if (!kEvt.m_bDown) 
		{
			m_pKeysPressedMap[kEvt.m_uiKey] = 0;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

char GfKeyboard::KeyToChar(GfKey::Type eKey, bool bShifted)
{
	if (eKey < GfKey::LAST_CHAR)
	{
		if (!bShifted) 
		{
			return g_pKeyToChar[eKey].m_siChar;
		}
		return g_pKeyToChar[eKey].m_siShiftedChar;;
	}
	return '\0';
}

////////////////////////////////////////////////////////////////////////////////

void GfKeyboard::SetKeyDown(GfKey::Type eKey)
{
	static_assert(GfKey::COUNT < (1 << 7), "This should never happen unless the table grows");
	KeyEvent kEvt;
	kEvt.m_bDown = 1;
	kEvt.m_uiKey = (eKey & 0x7F);
	m_tEventsToProcess.push(kEvt);
}

////////////////////////////////////////////////////////////////////////////////

void GfKeyboard::SetKeyUp(GfKey::Type eKey)
{
	static_assert(GfKey::COUNT < (1 << 7), "This should never happen unless the table grows");
	KeyEvent kEvt;
	kEvt.m_bDown = 0;
	kEvt.m_uiKey = (eKey & 0x7F);
	m_tEventsToProcess.push(kEvt);
}

////////////////////////////////////////////////////////////////////////////////
// EOF