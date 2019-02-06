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
#include "GfRender/Common/GfRenderContext.h"
#include "GfRender/Common/GfRenderCommon.h"

////////////////////////////////////////////////////////////////////////////////
// GfMatUniformFactory_Platform

GfMatUniformFactory_Platform::GfMatUniformFactory_Platform(GfMatUniformFactory& kBase)
	: m_kBase(kBase)
	, m_pPool(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfMatUniformFactory_Platform::CreateRHI(const GfRenderContext& kCtxt)
{
	GF_ASSERT(m_pPool, "Pool already initialized");

	VkDescriptorPoolSize pUniformEntryDescs[EParamaterSlotType::Count];
	
	VkDescriptorPoolSize* pPivot = pUniformEntryDescs;
	for (u32 i = 0; i < EParamaterSlotType::Count; ++i) 
	{
		if (m_kBase.m_pCountPerUniformType[i] > 0) 
		{
			pPivot->descriptorCount = m_kBase.m_pCountPerUniformType[i];
			pPivot->type = ConvertDescriptorType((EParamaterSlotType::Type)i);
			pPivot++;
		}
	}

	VkDescriptorPoolCreateInfo descPoolInfo{};
	descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descPoolInfo.pNext = nullptr;
	descPoolInfo.flags = 0;
	descPoolInfo.poolSizeCount = m_kBase.GetUsedTypeCount();
	descPoolInfo.pPoolSizes = pUniformEntryDescs;
	descPoolInfo.maxSets = m_kBase.m_uiMaxAllocatedSets;

	VkResult eResult = vkCreateDescriptorPool(kCtxt.m_pDevice, &descPoolInfo, nullptr, &m_pPool);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create the descriptor pool");
}

////////////////////////////////////////////////////////////////////////////////

void GfMatUniformFactory_Platform::DestroyRHI(const GfRenderContext& kCtxt)
{
	if (m_pPool) 
	{
		vkDestroyDescriptorPool(kCtxt.m_pDevice, m_pPool, nullptr);
		m_pPool = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////
// GfMatParamLayout_Platform

GfMatParamLayout_Platform::GfMatParamLayout_Platform(GfMatParamLayout& kBase)
	: m_kBase(kBase)
	, m_pSetLayout(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfMatParamLayout_Platform::CreateRHI(const GfRenderContext& kCtxt)
{
	u32 uiElementCount(static_cast<u32>(m_kBase.m_tParameters.size()));
	if (uiElementCount > 0) 
	{
		GfFrameMTStackAlloc::GfMemScope kMemScope(GfFrameMTStackAlloc::Get());

		VkDescriptorSetLayoutBinding* pBindings(GfFrameMTStackAlloc::Get()->Alloc<VkDescriptorSetLayoutBinding>(uiElementCount));
		VkDescriptorSetLayoutBinding* pPivot(pBindings);
		for (const GfMaterialParameterSlot& kSlot : m_kBase.m_tParameters) 
		{
			pBindings->binding = kSlot.m_uiBindSlot;
			pBindings->descriptorType = ConvertDescriptorType(kSlot.m_eType);
			pBindings->descriptorCount = 1; // TODO: Add support for arrays
			pBindings->stageFlags = ConvertShaderStageFlags(kSlot.m_AccesStages);
			pBindings->pImmutableSamplers = nullptr; // TODO: Add support for immutable samplers
			pBindings++;
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = uiElementCount;
		layoutInfo.pBindings = pBindings;

		VkResult siResult = vkCreateDescriptorSetLayout(kCtxt.m_pDevice, &layoutInfo, nullptr, &m_pSetLayout);
		GF_ASSERT(siResult == VK_SUCCESS, "Failed to create the descriptor set layout");
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfMatParamLayout_Platform::DestroyRHI(const GfRenderContext& kCtxt)
{
	if (m_pSetLayout) 
	{
		vkDestroyDescriptorSetLayout(kCtxt.m_pDevice, m_pSetLayout, nullptr);
		m_pSetLayout = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////
// GfMaterialParamSet_Platform

GfMaterialParamSet_Platform::GfMaterialParamSet_Platform(GfMaterialParamSet& kBase)
	: m_kBase(kBase)
	, m_pParamatersSet(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

bool GfMaterialParamSet_Platform::CreateRHI(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory)
{
	if (m_pParamatersSet) 
	{
		return false;
	}

	VkDescriptorSetAllocateInfo kDescSetAllocInfo{};
	kDescSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	kDescSetAllocInfo.pNext = nullptr;
	kDescSetAllocInfo.descriptorPool = kFactory.GetPool();
	kDescSetAllocInfo.descriptorSetCount = 1;

	VkDescriptorSetLayout pSetLayout(m_kBase.m_pSetLayout->GetLayout());
	kDescSetAllocInfo.pSetLayouts = &pSetLayout;
	VkResult eResult = vkAllocateDescriptorSets(kCtxt.m_pDevice, &kDescSetAllocInfo, &m_pParamatersSet);
	return eResult == VK_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamSet_Platform::DestroyRHI(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory)
{
	if (m_pParamatersSet) 
	{
		vkFreeDescriptorSets(kCtxt.m_pDevice, kFactory.GetPool(), 1, &m_pParamatersSet);
		m_pParamatersSet = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamSet_Platform::UpdateRHI(const GfRenderContext& kCtxt)
{

}

////////////////////////////////////////////////////////////////////////////////
// EOF