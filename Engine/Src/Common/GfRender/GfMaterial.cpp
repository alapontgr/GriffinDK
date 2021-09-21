////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/26
//	File: 	GfMaterial.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GfMaterial.h"

////////////////////////////////////////////////////////////////////////////////
// MaterialTemplate

GF_DEFINE_BASE_CTOR(GfMaterialTemplate)
	, m_pMaterialPass(nullptr)
	, m_uiConstantsStages(0)
	, m_uiConstantsBlockSize(0)
	, m_uiFlags(0)
{
	for (u32 i = 0; i < MaterialParamRate::MaxBoundSets; ++i)
	{
		m_pLayouts[i] = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialTemplate::Create(const GfRenderContext& kCtx)
{
	// If the material's gpu resource was already created, destroy it
	if (m_uiFlags.IsEnable(EFlags::GPUresource_Initialised))
	{
		Destroy(kCtx);
	}
	// Try to create the GPU resource
	if (m_kPlatform.createRHI(kCtx)) 
	{
		m_uiFlags |= EFlags::GPUresource_Initialised;
	}
	else 
	{
		// If we fail in the creation process, destroy all the gpu resources that may have been created
		Destroy(kCtx);
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialTemplate::Destroy(const GfRenderContext& kCtx)
{
	m_kPlatform.destroyRHI(kCtx);
	m_uiFlags &= ~EFlags::GPUresource_Initialised;
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialTemplate::AssignLayout(u32 uiSlot, const GfMatParamLayout* pLayout)
{
	GF_ASSERT(uiSlot<MaterialParamRate::MaxBoundSets, "Trying to bind a layout to an incorrect slot");
	if (uiSlot<MaterialParamRate::MaxBoundSets)
	{
		m_pLayouts[uiSlot] = pLayout;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialTemplate::SetShaderData(ShaderStage::Type eStage, const char* szEntry, const char* pSrc, u32 uiSrcDataSize)
{
	if (szEntry && pSrc) 
	{
		m_pShaderStages[eStage].m_pSourceData = pSrc;
		m_pShaderStages[eStage].m_szEntryPoint = szEntry;
		m_pShaderStages[eStage].m_uiSrcDataSize = uiSrcDataSize;
	}
}

////////////////////////////////////////////////////////////////////////////////

u32 GfMaterialTemplate::GetBoundLayoutCount() const
{
	u32 uiCount(0);
	for (u32 i = 0; i < MaterialParamRate::MaxBoundSets; ++i)
	{
		if (m_pLayouts[i]) 
		{
			uiCount++;
		}
	}
	return uiCount;
}

////////////////////////////////////////////////////////////////////////////////
// MaterialInstance

GfMaterialinstance::GfMaterialinstance()
	: m_pTemplate(nullptr)
{
	for (u32 i=0; i<MaterialParamRate::MaxBoundSets; ++i) 
	{
		m_pBoundSets[i] = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF