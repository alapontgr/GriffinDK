////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/07
//	File: 	GfMaterialParameterBlock.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMATERIALPARAMETERBLOCK_H__
#define __GFMATERIALPARAMETERBLOCK_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfCore/Common/GfStl.h"
#include "GfRender/Common/GfMaterialShared.h"
#include GF_SOLVE_PLATFORM_HEADER(GfMaterialParameterBlock)

////////////////////////////////////////////////////////////////////////////////

struct GfMaterialParameterSlot
{
	EParamaterSlotType::Type	m_eType;
	EMaterialParamRate::Type	m_eRate;
	GfShaderAccessMask			m_AccesStages;
	u32							m_uiBindSlot;
};

////////////////////////////////////////////////////////////////////////////////

class GfMaterialParameterBlock : public GfMaterialParameterBlock_Platform
{
public:

	friend class GfMaterialParameterBlock_Platform;

	GfMaterialParameterBlock();

	void DefineParameter(EParamaterSlotType::Type eType, GfShaderAccessMask mAccessMask, u32 uiBindSlot);

private:

	GfVector<GfMaterialParameterSlot> m_tParameters;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIALPARAMETERBLOCK_H__