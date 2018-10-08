////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/07
//	File: 	GfMaterialParameterBlock.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMATERIALPARAMLAYOUT_H__
#define __GFMATERIALPARAMLAYOUT_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfCore/Common/GfStl.h"
#include "GfRender/Common/GfMaterialShared.h"
#include GF_SOLVE_PLATFORM_HEADER(GfMaterialParamLayout)

////////////////////////////////////////////////////////////////////////////////

struct GfMaterialParameterSlot
{
	EParamaterSlotType::Type	m_eType;
	EMaterialParamRate::Type	m_eRate;
	GfShaderAccessMask			m_AccesStages;
	u32							m_uiBindSlot;
};

////////////////////////////////////////////////////////////////////////////////

class GfMaterialParamLayout : public GfMaterialParamLayout_Platform
{
public:

	friend class GfMaterialParamLayout_Platform;

	GfMaterialParamLayout();

	void DefineParameter(EParamaterSlotType::Type eType, GfShaderAccessMask mAccessMask, u32 uiBindSlot);

	u8 GetBindSlotForRate(EMaterialParamRate::Type eRate);

	u8 GetParamBlockStartOffset(EMaterialParamRate::Type eRate);

	static u8 ms_uiInvalidValue;

private:

	void FixParametersLayout();

	GfVector<GfMaterialParameterSlot>	m_tParameters;
	// For the different rates of parameters the index of the first parameter
	u8									m_pSetsOffsets[EMaterialParamRate::Count];
	// Defines where each block of parameters will be bound
	u8									m_pSetsBindSlot[EMaterialParamRate::Count];
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u8 GfMaterialParamLayout::GetBindSlotForRate(EMaterialParamRate::Type eRate)
{
	return m_pSetsBindSlot[eRate];
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u8 GfMaterialParamLayout::GetParamBlockStartOffset(EMaterialParamRate::Type eRate)
{
	return m_pSetsOffsets[eRate];
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIALPARAMETERBLOCK_H__