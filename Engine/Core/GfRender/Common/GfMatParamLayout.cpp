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
	, m_uiFlags(0)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfMatParamLayout::Create(const GfRenderContext& kCtxt)
{
	if (m_kPlatform.CreateRHI(kCtxt)) 
	{
		m_uiFlags |= EFlags::GPU_Initialised;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfMatParamLayout::Destroy(const GfRenderContext& kCtxt)
{
	m_kPlatform.DestroyRHI(kCtxt);
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

u32 GfMatParamLayout::GetParameterCount() const
{
	return static_cast<u32>(m_tParameters.size());
}

////////////////////////////////////////////////////////////////////////////////

GfMaterialParameterSlot GfMatParamLayout::GetAttrib(GfParamSlotIdx uiSlot) const
{
	return m_tParameters[uiSlot];
}

////////////////////////////////////////////////////////////////////////////////

GfParamSlotIdx GfMatParamLayout::GetResourceIdx(EParamaterSlotType::Type eResType, u32 uiBoundSlot)
{
	u32 uiParamCount(GetParameterCount());
	for (u32 i=0; i<uiParamCount; ++i)
	{
		const GfMaterialParameterSlot& kSlot = m_tParameters[i];
		if (kSlot.m_eType == eResType && kSlot.m_uiBindSlot == uiBoundSlot)
		{
			return i;
		}
	}
	return GfMaterialParameterSlot::ms_uiInvalidSlotIdx;
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
	GF_ASSERT(m_uiFlags.IsEnable(EFlags::LayoutAssigned) && !m_uiFlags.IsEnable(EFlags::GPUResourceInitialised), "Unable to create MaterialParamSet");
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

void GfMaterialParamSet::Init(const GfMatParamLayout* pParamLayout)
{
	GF_ASSERT(!m_pSetLayout, "TEMPORARY: Do some protection to avoid re-initialization");
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

void GfMaterialParamSet::BindResource(GfParamSlotIdx uiSlotIdx, const GfGraphicsResourceBase* pResource)
{
	if (uiSlotIdx != GfMaterialParameterSlot::ms_uiInvalidSlotIdx)
	{
		if (pResource && m_uiFlags.IsEnable(EFlags::LayoutAssigned))
		{
			GF_ASSERT(uiSlotIdx < ms_uiMaxResources, "Trying to bind to an invalid slot");

			// Mark the GPU's resource as dirty
			m_uiFlags |= EFlags::GPUUpdatePending;
			m_tBoundParamaters[uiSlotIdx] = pResource;
			m_uiDirtyResources |= (1 << uiSlotIdx);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF