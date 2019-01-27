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

void GfVertexDeclaration::Init(GfVertexAttrib* pAttributes, u16 uiAttribCount, u16 uiStride)
{
	GF_ASSERT(m_uiAttribCount == 0, "Already initialised");
	GF_ASSERT(uiAttribCount <= ms_uiVertexMaxAttribCount, "Trying to assign too many attributes");
	m_uiAttribCount = uiAttribCount;
	m_uiVertexStride = uiStride;
	// Copy attributes
	for (u32 i = 0; i < uiAttribCount; ++i) 
	{
		m_pAttribs[i] = pAttributes[i];
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF