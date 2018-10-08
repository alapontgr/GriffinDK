////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/07
//	File: 	GfMaterialParameterBlock.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfMaterialParamLayout.h"

////////////////////////////////////////////////////////////////////////////////

u8 GfMaterialParamLayout::ms_uiInvalidValue = 0xff;

////////////////////////////////////////////////////////////////////////////////

GfMaterialParamLayout::GfMaterialParamLayout()
	: GfMaterialParamLayout_Platform(*this)
{

}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamLayout::DefineParameter(EParamaterSlotType::Type eType, GfShaderAccessMask mAccessMask, u32 uiBindSlot)
{

}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamLayout::FixParametersLayout()
{
	// Reset the state of the tables
	for (u32 i = 0; i < EMaterialParamRate::Count; ++i) 
	{
		m_pSetsOffsets[i] = ms_uiInvalidValue;
		m_pSetsBindSlot[i] = ms_uiInvalidValue;
	}

	if (m_tParameters.size() > 0) 
	{
		std::sort(m_tParameters.begin(), m_tParameters.end(), [&](const GfMaterialParameterSlot& kA, const GfMaterialParameterSlot& kB)
		{
			if (kA.m_eRate != kB.m_eRate)
			{
				return kA.m_eRate <= kB.m_eRate;
			}
			return kA.m_uiBindSlot <= kB.m_uiBindSlot;
		});

		// Resolve the beginning of each parameter block
		u8 uiCurrCursor(0);
		for (GfMaterialParameterSlot& kEntry : m_tParameters) 
		{
			if (m_pSetsOffsets[kEntry.m_eRate] == ms_uiInvalidValue)
			{
				m_pSetsOffsets[kEntry.m_eRate] = uiCurrCursor;
			}
			uiCurrCursor++;
		}

		// Resolve the binding points
		u8 uiCurrBindSlot(0);
		for (u32 i = 0; i<EMaterialParamRate::Count; ++i)
		{
			if (m_pSetsOffsets[i] != ms_uiInvalidValue)
			{
				m_pSetsOffsets[i] = uiCurrBindSlot;
				++uiCurrBindSlot;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF