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

#include "GfRender/Common/GfMatParamLayout.h"

////////////////////////////////////////////////////////////////////////////////

u8 GfMatParamLayout::ms_uiInvalidValue = 0xff;

////////////////////////////////////////////////////////////////////////////////

GfMatUniformFactory::GfMatUniformFactory()
	: GfMatUniformFactory_Platform(*this)
{
	for (u32 i = 0; i < EParamaterSlotType::Count; ++i) 
	{
		m_pCountPerUniformType[i] = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////

GfMatParamLayout::GfMatParamLayout()
	: GfMatParamLayout_Platform(*this)
{

}

////////////////////////////////////////////////////////////////////////////////

void GfMatParamLayout::DefineParameter(EParamaterSlotType::Type eType, GfShaderAccessMask mAccessMask, u32 uiBindSlot)
{

}

////////////////////////////////////////////////////////////////////////////////

void GfMatParamLayout::SortParameters()
{
	if (m_tParameters.size() > 0) 
	{
		std::sort(m_tParameters.begin(), m_tParameters.end(), [&](const GfMaterialParameterSlot& kA, const GfMaterialParameterSlot& kB)
		{
			// Sort per type first and per bind slot after
			if (kA.m_eType != kB.m_eType)
			{
				return kA.m_eType <= kB.m_eType;
			}
			return kA.m_uiBindSlot <= kB.m_uiBindSlot;
		});
	}
}

////////////////////////////////////////////////////////////////////////////////

GfMaterialParamGroup::GfMaterialParamGroup()
	: GfMaterialParamGroup_Platform(*this)
{
	for (u32 i=0; i<EMaterialParamRate::Count; ++i) 
	{
		m_pPerRateLayouts[i] = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamGroup::BindLayout(const GfMatParamLayout* pParamLayout, EMaterialParamRate::Type eRate)
{
	m_pPerRateLayouts[eRate] = pParamLayout;
	// TODO: Signal creation
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamGroup::Create(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory)
{
	CreateRHI(kCtxt, kFactory);
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamGroup::Destroy(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory)
{
	DestroyRHI(kCtxt, kFactory);
}

////////////////////////////////////////////////////////////////////////////////
// EOF