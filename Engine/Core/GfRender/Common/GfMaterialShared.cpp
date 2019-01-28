////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/01/27
//	File: 	GfMaterialShared.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfMaterialShared.h"

////////////////////////////////////////////////////////////////////////////////

GfVertexDeclaration::GfVertexDeclaration()
	: m_uiAttribCount(0)
{}

////////////////////////////////////////////////////////////////////////////////

void GfVertexDeclaration::Init(GfVertexAttrib* pAttributes, u16 uiAttribCount, u16 uiStride, EVertexInputRate::Type eRate)
{
	GF_ASSERT(m_uiAttribCount == 0, "Already initialised");
	GF_ASSERT(uiAttribCount <= ms_uiVertexMaxAttribCount, "Trying to assign too many attributes");
	m_uiAttribCount = uiAttribCount;
	m_uiVertexStride = uiStride;
	m_eRate = eRate;
	// Copy attributes
	for (u32 i = 0; i < uiAttribCount; ++i) 
	{
		m_pAttribs[i] = pAttributes[i];
	}
}

////////////////////////////////////////////////////////////////////////////////

GfVertexDeclaration::GfVertexAttrib GfVertexDeclaration::GetAttrib(u32 uiSlot) const
{
	GF_ASSERT(uiSlot < m_uiAttribCount, "Invalid slot");
	return m_pAttribs[uiSlot];
}

////////////////////////////////////////////////////////////////////////////////
// EOF