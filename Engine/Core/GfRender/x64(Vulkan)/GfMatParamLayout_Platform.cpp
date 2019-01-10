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

////////////////////////////////////////////////////////////////////////////////

static const VkDescriptorType g_pDescriptorTypeConverter[] = 
{
	VK_DESCRIPTOR_TYPE_SAMPLER,
	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
	VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
	VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
	VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
	VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
	VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
};

////////////////////////////////////////////////////////////////////////////////

// Direct map for the moment
static inline VkDescriptorType ConvertParamaterType(EParamaterSlotType::Type eType) 
{
	return g_pDescriptorTypeConverter[eType];
}

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
			pPivot->type = ConvertParamaterType((EParamaterSlotType::Type)i);
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
	GF_ASSERT_ALWAYS("Implement me!!!");
}

////////////////////////////////////////////////////////////////////////////////

void GfMatParamLayout_Platform::DestroyRHI(const GfRenderContext& kCtxt)
{
	GF_ASSERT_ALWAYS("Implement me!!!");
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
// EOF