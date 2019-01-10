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
// GfMatUniformFactory

GfMatUniformFactory::GfMatUniformFactory()
	: m_uiMaxAllocatedSets(0)
	, GfMatUniformFactory_Platform(*this)
{
	for (u32 i = 0; i < EParamaterSlotType::Count; ++i) 
	{
		m_pCountPerUniformType[i] = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfMatUniformFactory::SetMaxAllocationsPerParamType(EParamaterSlotType::Type eType, u32 uiMaxAllocations)
{
	m_pCountPerUniformType[eType] = uiMaxAllocations;
}

////////////////////////////////////////////////////////////////////////////////

void GfMatUniformFactory::SetMaxAllocatedParamSets(u32 uiMaxSets)
{
	m_uiMaxAllocatedSets = uiMaxSets;
}

////////////////////////////////////////////////////////////////////////////////

void GfMatUniformFactory::Create(const GfRenderContext& kCtxt)
{
	CreateRHI(kCtxt);
}

////////////////////////////////////////////////////////////////////////////////

void GfMatUniformFactory::Destroy(const GfRenderContext& kCtxt)
{
	DestroyRHI(kCtxt);
}

////////////////////////////////////////////////////////////////////////////////

u32 GfMatUniformFactory::GetUsedTypeCount() const
{
	u32 uiUsed(0);
	for (u32 i=0; i<EParamaterSlotType::Count; ++i) 
	{
		if (m_pCountPerUniformType[i] > 0) 
		{
			uiUsed++;
		}
	}
	return uiUsed;
}

////////////////////////////////////////////////////////////////////////////////
// GfMatParamLayout

GfMatParamLayout::GfMatParamLayout()
	: GfMatParamLayout_Platform(*this)
{

}

////////////////////////////////////////////////////////////////////////////////

void GfMatParamLayout::DefineParameter(EParamaterSlotType::Type eType, GfShaderAccessMask mAccessMask, u32 uiBindSlot)
{
	GfMaterialParameterSlot kSlot;
	kSlot.m_eType = eType;
	kSlot.m_AccesStages = mAccessMask;
	kSlot.m_uiBindSlot = uiBindSlot;
	m_tParameters.push_back(kSlot);
}

////////////////////////////////////////////////////////////////////////////////

bool GfMatParamLayout::Validate() const
{
	// Check for parameters of the same type sharing slots
	for (const GfMaterialParameterSlot& kSlotA : m_tParameters)
	{
		for (const GfMaterialParameterSlot& kSlotB : m_tParameters)
		{
			if (kSlotA.m_eType == kSlotB.m_eType && kSlotA.m_uiBindSlot == kSlotB.m_uiBindSlot) 
			{
				return false;
			}
		}
	}
	return true;
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
// GfMaterialParamSet

GfMaterialParamSet::GfMaterialParamSet()
	: GfMaterialParamSet_Platform(*this)
	, m_pSetLayout(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamSet::BindLayout(const GfMatParamLayout* pParamLayout)
{
	m_pSetLayout = pParamLayout;
}

////////////////////////////////////////////////////////////////////////////////

bool GfMaterialParamSet::Create(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory)
{
	return CreateRHI(kCtxt, kFactory);
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamSet::Destroy(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory)
{
	DestroyRHI(kCtxt, kFactory);
}

////////////////////////////////////////////////////////////////////////////////
// EOF