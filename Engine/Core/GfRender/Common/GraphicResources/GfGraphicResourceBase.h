////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/06
//	File: 	GfGraphicResourceBase.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFGRAPHICRESOURCEBASE_H__
#define __GFGRAPHICRESOURCEBASE_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfRender/Common/GfMaterialShared.h"

////////////////////////////////////////////////////////////////////////////////

class GfGraphicsResourceBase 
{
public:

	GfGraphicsResourceBase();

	GfGraphicsResourceBase(EParamaterSlotType::Type eType);

	EParamaterSlotType::Type GetResourceType() const { return m_eType; }

protected:

	enum EFlags : u16 
	{
		BufferBound = 1<<0,
		GPUDirty	= 1<<1,
		GPUPending	= 1<<2,
	};

	GfBitMask<u16>				m_uiFlags;
	EParamaterSlotType::Type	m_eType;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFGRAPHICRESOURCEBASE_H__