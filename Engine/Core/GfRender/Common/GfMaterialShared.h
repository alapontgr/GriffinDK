////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/07
//	File: 	GfMaterialShared.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMATERIALSHARED_H__
#define __GFMATERIALSHARED_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfRender/Common/GfRenderCommon.h"

////////////////////////////////////////////////////////////////////////////////

class GfVertexDeclaration 
{
public:

	struct GfVertexAttrib 
	{
		u16						m_uiOffset;	// Offset within the struct where this attrib is
		EAttributeFormat::Type	m_eType;	// Type of the attribute
	};

	GfVertexDeclaration();

	void Init(GfVertexAttrib* pAttributes, u32 uiAttribCount, u32 uiStride, EVertexInputRate::Type eRate);

	GfVertexAttrib GetAttrib(u32 uiSlot) const;

	EVertexInputRate::Type GetRate() const { return m_eRate; }

	u32 GetAttribCount() const { return m_uiAttribCount; }

	u32 GetStride() const { return m_uiVertexStride; }

	static constexpr u32 ms_uiVertexMaxAttribCount = 6;

private:

	GfVertexAttrib			m_pAttribs[ms_uiVertexMaxAttribCount];
	u32						m_uiAttribCount;
	u32						m_uiVertexStride;
	EVertexInputRate::Type	m_eRate;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIALSHARED_H__