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

GF_DEFINE_BASE_CTOR(GfMatUniformFactory)
	, m_uiMaxAllocatedSets(0)
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
	m_kPlatform.CreateRHI(kCtxt);
}

////////////////////////////////////////////////////////////////////////////////

void GfMatUniformFactory::Destroy(const GfRenderContext& kCtxt)
{
	m_kPlatform.DestroyRHI(kCtxt);
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

GF_DEFINE_BASE_CTOR(GfMatParamLayout)
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
	for (size_t i = 0; i < m_tParameters.size(); ++i)
	{	
		const GfMaterialParameterSlot& kSlotA(m_tParameters[i]);
		for (size_t j = i+1; j < m_tParameters.size(); ++j)
		{
			const GfMaterialParameterSlot& kSlotB(m_tParameters[j]);
			if (kSlotA.m_eType == kSlotB.m_eType && kSlotA.m_uiBindSlot >= kSlotB.m_uiBindSlot)
			{
				return false;
			}
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

u32 GfMatParamLayout::GetParameterCount() const
{
	return static_cast<u32>(m_tParameters.size());
}

////////////////////////////////////////////////////////////////////////////////

GfMaterialParameterSlot GfMatParamLayout::GetAttrib(u32 uiSlot) const
{
	return m_tParameters[uiSlot];
}

////////////////////////////////////////////////////////////////////////////////
// GfMaterialParamSet

GF_DEFINE_BASE_CTOR(GfMaterialParamSet)
	, m_pSetLayout(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

bool GfMaterialParamSet::Create(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory)
{
	if (m_uiFlags.IsEnable(EFlags::LayoutAssigned) && !m_uiFlags.IsEnable(EFlags::GPUResourceInitialised))
	{
		if (m_kPlatform.CreateRHI(kCtxt, kFactory))
		{
			m_uiFlags |= EFlags::GPUResourceInitialised;
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamSet::Destroy(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory)
{
	m_kPlatform.DestroyRHI(kCtxt, kFactory);
}

////////////////////////////////////////////////////////////////////////////////

bool GfMaterialParamSet::Update(const GfRenderContext& kCtxt)
{
	// Check that there are some resources pending to be updated
	if (m_uiFlags.IsEnable(EFlags::GPUUpdatePending | EFlags::GPUResourceInitialised)) 
	{
		m_kPlatform.UpdateRHI(kCtxt);
		m_uiFlags &= ~(EFlags::GPUUpdatePending | EFlags::GPUDirty);
		m_uiDirtyResources = 0;
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamSet::BindLayout(const GfMatParamLayout* pParamLayout)
{
	GF_ASSERT(m_pSetLayout, "TEMPORARY: Do some protection to avoid re-initialization");
	m_pSetLayout = pParamLayout;
	m_uiFlags |= (EFlags::LayoutAssigned | EFlags::GPUDirty);
	
	u32 uiParamCount(m_pSetLayout->GetParameterCount());
	m_tBoundParamaters.clear();
	m_tBoundParamaters.resize(uiParamCount);
	
	// Clear the state of the Parameter Set (No parameters bound at this point)
	for (u32 i=0; i<uiParamCount; ++i)
	{
		m_tBoundParamaters[i] = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamSet::BindResource(u32 uiSlot, const GfGraphicsResourceBase* pResource)
{
	if (pResource && m_uiFlags.IsEnable(EFlags::LayoutAssigned))
	{
		GF_ASSERT(uiSlot < ms_uiMaxResources, "Trying to bind to an invalid slot");

		// Mark the GPU's resource as dirty
		m_uiFlags |= EFlags::GPUUpdatePending;
		m_tBoundParamaters[uiSlot] = pResource;
		m_uiDirtyResources |= (1 << uiSlot);
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF