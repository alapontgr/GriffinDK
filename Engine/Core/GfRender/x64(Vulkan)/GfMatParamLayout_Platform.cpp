////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/07
//	File: 	GfMaterialParameterBlock_Platform.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfMatParamLayout.h"

////////////////////////////////////////////////////////////////////////////////

GfMatUniformFactory_Platform::GfMatUniformFactory_Platform(GfMatUniformFactory& kBase)
	: m_kBase(kBase)
	, m_pPool(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfMatUniformFactory_Platform::CreateRHI(const GfRenderContext& kCtxt)
{
	GF_ASSERT_ALWAYS("Implement me!!!");
}

////////////////////////////////////////////////////////////////////////////////

void GfMatUniformFactory_Platform::DestroyRHI(const GfRenderContext& kCtxt)
{
	GF_ASSERT_ALWAYS("Implement me!!!");
}

////////////////////////////////////////////////////////////////////////////////

GfMatParamLayout_Platform::GfMatParamLayout_Platform(GfMatParamLayout& kBase)
	: m_kBase(kBase)
	, m_pSetLayout(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

GfMaterialParamGroup_Platform::GfMaterialParamGroup_Platform(GfMaterialParamGroup& kBase)
	: m_kBase(kBase)
	, m_pParamatersGroup(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamGroup_Platform::CreateRHI(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory)
{
	VkDescriptorSetLayout pLayouts[EMaterialParamRate::Count];
	u32 uiCursor(0);
	for (u32 i = 0; i < EMaterialParamRate::Count; ++i) 
	{
		if (m_kBase.m_kBase.m_pPerRateLayouts[i]) 
		{
			pLayouts[uiCursor++] = m_kBase.m_kBase.m_pPerRateLayouts[i]->GetLayout();
		}
	}
	// TODO
	GF_ASSERT_ALWAYS("Implement me!!!");
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamGroup_Platform::DestroyRHI(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory)
{
	if (m_pParamatersGroup) 
	{
		GF_ASSERT_ALWAYS("Implement me!!!");
		// TODO
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF