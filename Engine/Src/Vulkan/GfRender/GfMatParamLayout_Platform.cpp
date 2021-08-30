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

#include "Common/GfRender/GfMatParamLayout.h"
#include "Common/GfRender/GfRenderContext.h"
#include "Common/GfRender/GfRenderCommon.h"
#include "Common/GfRender/GfCmdBuffer.h"
#include "Common/GfRender/GfMaterial.h"
#include "Common/GfRender/GraphicResources/GfGraphicResources.h"

////////////////////////////////////////////////////////////////////////////////

void FillUniformBufferBinding(
	const GfBufferedResource* pCBuffer,
	VkDescriptorBufferInfo* pBufferInfosPivot,
	VkWriteDescriptorSet* pWriteSetsPivot,
	VkDescriptorSet pSet, u32 uiBindSlot)
{
	const GfBuffer::GfRange kRange(pCBuffer->GetBufferRange());
	GF_ASSERT(kRange.m_pBuffer->isGPUReady(), "Buffer has not been created yet");
	pBufferInfosPivot->buffer = kRange.m_pBuffer->Plat().GetHandle();
	pBufferInfosPivot->offset = kRange.m_uiOffset;
	pBufferInfosPivot->range = kRange.m_uiSize;

	pWriteSetsPivot->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	pWriteSetsPivot->pNext = nullptr;
	pWriteSetsPivot->dstSet = pSet;
	pWriteSetsPivot->dstBinding = uiBindSlot;
	pWriteSetsPivot->dstArrayElement = 0;
	pWriteSetsPivot->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pWriteSetsPivot->descriptorCount = 1;
	pWriteSetsPivot->pBufferInfo = pBufferInfosPivot;
	pWriteSetsPivot->pImageInfo = nullptr;
	pWriteSetsPivot->pTexelBufferView = nullptr;
}

////////////////////////////////////////////////////////////////////////////////

void FillTextureBinding(
	const GfTextureView* pView,
	VkDescriptorImageInfo* pImageInfosPivot,
	VkWriteDescriptorSet* pWriteSetsPivot,
	VkDescriptorSet pSet, u32 uiBindSlot)
{
	GF_ASSERT(pView->IsGPUReady(), "Texture view not created");
	pImageInfosPivot->sampler = nullptr;
	pImageInfosPivot->imageView = pView->Plat().GetView();
	pImageInfosPivot->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // The texture must be initialized at this point

	pWriteSetsPivot->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	pWriteSetsPivot->pNext = nullptr;
	pWriteSetsPivot->dstSet = pSet;
	pWriteSetsPivot->dstBinding = uiBindSlot;
	pWriteSetsPivot->dstArrayElement = 0;
	pWriteSetsPivot->descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	pWriteSetsPivot->descriptorCount = 1;
	pWriteSetsPivot->pBufferInfo = nullptr;
	pWriteSetsPivot->pImageInfo = pImageInfosPivot;
	pWriteSetsPivot->pTexelBufferView = nullptr;
}

////////////////////////////////////////////////////////////////////////////////

void FillSamplerStateBinding(
	const GfSamplerState* pSampler,
	VkDescriptorImageInfo* pImageInfosPivot,
	VkWriteDescriptorSet* pWriteSetsPivot,
	VkDescriptorSet pSet, u32 uiBindSlot) 
{
	GF_ASSERT(pSampler->IsGPUReady(), "Sampler not created");
	pImageInfosPivot->sampler = pSampler->Plat().GetSampler();
	pImageInfosPivot->imageView = nullptr;
	
	pWriteSetsPivot->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	pWriteSetsPivot->pNext = nullptr;
	pWriteSetsPivot->dstSet = pSet;
	pWriteSetsPivot->dstBinding = uiBindSlot;
	pWriteSetsPivot->dstArrayElement = 0;
	pWriteSetsPivot->descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	pWriteSetsPivot->descriptorCount = 1;
	pWriteSetsPivot->pBufferInfo = nullptr;
	pWriteSetsPivot->pImageInfo = pImageInfosPivot;
	pWriteSetsPivot->pTexelBufferView = nullptr;
}

////////////////////////////////////////////////////////////////////////////////

void FillCombinedSamplerTextureStateBinding(
	const GfCombinedSamplerTexture* pCombinedSamplerTexture,
	VkDescriptorImageInfo* pImageInfosPivot,
	VkWriteDescriptorSet* pWriteSetsPivot,
	VkDescriptorSet pSet, u32 uiBindSlot)
{
	GF_ASSERT(pCombinedSamplerTexture->GetView()->IsGPUReady(), "Texture view not created");
	GF_ASSERT(pCombinedSamplerTexture->GetSampler()->IsGPUReady(), "Sampler not created");

	pImageInfosPivot->sampler = pCombinedSamplerTexture->GetSampler()->Plat().GetSampler();
	pImageInfosPivot->imageView = pCombinedSamplerTexture->GetView()->Plat().GetView();
	pImageInfosPivot->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // The texture must be initialized at this point

	pWriteSetsPivot->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	pWriteSetsPivot->pNext = nullptr;
	pWriteSetsPivot->dstSet = pSet;
	pWriteSetsPivot->dstBinding = uiBindSlot;
	pWriteSetsPivot->dstArrayElement = 0;
	pWriteSetsPivot->descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pWriteSetsPivot->descriptorCount = 1;
	pWriteSetsPivot->pBufferInfo = nullptr;
	pWriteSetsPivot->pImageInfo = pImageInfosPivot;
	pWriteSetsPivot->pTexelBufferView = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// GfMatUniformFactory_Platform

GfMatUniformFactory_Platform::GfMatUniformFactory_Platform(GfMatUniformFactory& kBase)
	: m_kBase(kBase)
	, m_pPool(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfMatUniformFactory_Platform::createRHI(const GfRenderContext& kCtxt)
{
	GF_ASSERT(!m_pPool, "Pool already initialized");

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

	VkResult eResult = vkCreateDescriptorPool(kCtxt.Plat().m_pDevice, &descPoolInfo, nullptr, &m_pPool);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create the descriptor pool");
}

////////////////////////////////////////////////////////////////////////////////

void GfMatUniformFactory_Platform::destroyRHI(const GfRenderContext& kCtxt)
{
	if (m_pPool) 
	{
		vkDestroyDescriptorPool(kCtxt.Plat().m_pDevice, m_pPool, nullptr);
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

bool GfMatParamLayout_Platform::createRHI(const GfRenderContext& kCtxt)
{
	u32 uiElementCount(static_cast<u32>(m_kBase.m_tParameters.size()));
	if (uiElementCount > 0) 
	{
		GfFrameMTStackAlloc::GfMemScope kMemScope(GfFrameMTStackAlloc::Get());

		VkDescriptorSetLayoutBinding* pBindings(GfFrameMTStackAlloc::Get()->Alloc<VkDescriptorSetLayoutBinding>(uiElementCount));
		VkDescriptorSetLayoutBinding* pPivot(pBindings);
		for (const GfMaterialParameterSlot& kSlot : m_kBase.m_tParameters) 
		{
			pPivot->binding = kSlot.m_uiBindSlot;
			pPivot->descriptorType = ConvertDescriptorType(kSlot.m_eType);
			pPivot->descriptorCount = 1; // TODO: Add support for arrays
			pPivot->stageFlags = ConvertShaderStageFlags(kSlot.m_AccesStages);
			pPivot->pImmutableSamplers = nullptr; // TODO: Add support for immutable samplers
			pPivot++;
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = uiElementCount;
		layoutInfo.pBindings = pBindings;

		VkResult siResult = vkCreateDescriptorSetLayout(kCtxt.Plat().m_pDevice, &layoutInfo, nullptr, &m_pSetLayout);
		if (siResult == VK_SUCCESS) 
		{
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void GfMatParamLayout_Platform::destroyRHI(const GfRenderContext& kCtxt)
{
	if (m_pSetLayout) 
	{
		vkDestroyDescriptorSetLayout(kCtxt.Plat().m_pDevice, m_pSetLayout, nullptr);
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

bool GfMaterialParamSet_Platform::createRHI(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory)
{
	if (m_pParamatersSet) 
	{
		return false;
	}

	VkDescriptorSetAllocateInfo kDescSetAllocInfo{};
	kDescSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	kDescSetAllocInfo.pNext = nullptr;
	kDescSetAllocInfo.descriptorPool = kFactory.Plat().GetPool();
	kDescSetAllocInfo.descriptorSetCount = 1;

	VkDescriptorSetLayout pSetLayout(m_kBase.m_pSetLayout->Plat().GetLayout());
	kDescSetAllocInfo.pSetLayouts = &pSetLayout;
	VkResult eResult = vkAllocateDescriptorSets(kCtxt.Plat().m_pDevice, &kDescSetAllocInfo, &m_pParamatersSet);
	return eResult == VK_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamSet_Platform::destroyRHI(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory)
{
	if (m_pParamatersSet) 
	{
		vkFreeDescriptorSets(kCtxt.Plat().m_pDevice, kFactory.Plat().GetPool(), 1, &m_pParamatersSet);
		m_pParamatersSet = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamSet_Platform::UpdateRHI(const GfRenderContext& kCtxt)
{
	u32 uiParamCount(m_kBase.m_pSetLayout->GetParameterCount());
	
	u32 uiDirtyBufferedParams(0);
	u32 uiDirtyTexturedParams(0);

	// Get number of dirty resources of each write type
	for (u32 i=0;i<uiParamCount; ++i) 
	{
		if (m_kBase.m_uiDirtyResources & (1 << i))
		{
			const GfMaterialParameterSlot& kSlot(m_kBase.m_pSetLayout->GetAttrib(i));
			switch (kSlot.m_eType)
			{
			case EParamaterSlotType::UniformBuffer:
				uiDirtyBufferedParams++;
				break;
			case EParamaterSlotType::Texture:
			case EParamaterSlotType::Sampler:
			case EParamaterSlotType::CombinedTextureSampler:
				uiDirtyTexturedParams++;
				break;
			default:
				break;
			}
		}
	}

	u32 uiTotalDirtyResources(uiDirtyBufferedParams + uiDirtyTexturedParams);
	if (uiTotalDirtyResources > 0) 
	{
		GfFrameMTStackAlloc::GfMemScope kMemScope(GfFrameMTStackAlloc::Get());

		VkDescriptorBufferInfo* pBufferInfos(GfFrameMTStackAlloc::Get()->Alloc<VkDescriptorBufferInfo>(uiDirtyBufferedParams));
		GF_ASSERT(pBufferInfos, "Failed to allocate memory");
		VkDescriptorImageInfo* pImageInfos(GfFrameMTStackAlloc::Get()->Alloc<VkDescriptorImageInfo>(uiDirtyTexturedParams));
		GF_ASSERT(pImageInfos, "Failed to allocate memory");
		VkWriteDescriptorSet* pWriteSets(GfFrameMTStackAlloc::Get()->Alloc<VkWriteDescriptorSet>(uiTotalDirtyResources));
		GF_ASSERT(pWriteSets, "Failed to allocate memory");

		VkDescriptorBufferInfo* pBufferInfosPivot(pBufferInfos);
		VkDescriptorImageInfo* pImageInfosPivot(pImageInfos);
		VkWriteDescriptorSet* pWriteSetsPivot(pWriteSets);
		for (u32 i=0; i<uiParamCount; ++i) 
		{
			if (m_kBase.m_uiDirtyResources & (1 << i)) 
			{
				// TODO: Handle all the resource types
				const GfGraphicsResourceBase* pParam(m_kBase.m_tBoundParamaters[i]);
				const GfMaterialParameterSlot& kSlot(m_kBase.m_pSetLayout->GetAttrib(i));

				switch (kSlot.m_eType)
				{
				case EParamaterSlotType::UniformBuffer:
				{
					const GfBufferedResource* pCBuffer((const GfBufferedResource*)pParam);
					FillUniformBufferBinding(pCBuffer, pBufferInfosPivot, pWriteSetsPivot, m_pParamatersSet, kSlot.m_uiBindSlot);
					pBufferInfosPivot++;
					pWriteSetsPivot++;
					break;
				}
				case EParamaterSlotType::Texture: 
				{
					const GfTextureView* pTexRes((const GfTextureView*)pParam);
					FillTextureBinding(pTexRes, pImageInfosPivot, pWriteSetsPivot, m_pParamatersSet, kSlot.m_uiBindSlot);
					pImageInfosPivot++;
					pWriteSetsPivot++;
					break;
				}
				case EParamaterSlotType::Sampler: 
				{
					const GfSamplerState* pSampler((const GfSamplerState*)pParam);
					FillSamplerStateBinding(pSampler, pImageInfosPivot, pWriteSetsPivot, m_pParamatersSet, kSlot.m_uiBindSlot);
					pImageInfosPivot++;
					pWriteSetsPivot++;
					break;
				}
				case EParamaterSlotType::CombinedTextureSampler:
				{
					const GfCombinedSamplerTexture* pCombinedSamplerTexture((const GfCombinedSamplerTexture*)pParam);
					FillCombinedSamplerTextureStateBinding(
						pCombinedSamplerTexture, pImageInfosPivot, pWriteSetsPivot, m_pParamatersSet, kSlot.m_uiBindSlot);
					pImageInfosPivot++;
					pWriteSetsPivot++;
					break;
				}
				default:
					break;
				}
			}
		}
		vkUpdateDescriptorSets(kCtxt.Plat().m_pDevice, uiTotalDirtyResources, pWriteSets, 0, nullptr);
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfMaterialParamSet_Platform::BindRHI(const GfCmdBuffer& kCmdBuffer, const GfMaterialTemplate& kMaterial, u32 uiBindPoint, bool bIsGraphics)
{
	VkPipelineBindPoint uiBindType(bIsGraphics ? VK_PIPELINE_BIND_POINT_GRAPHICS : VK_PIPELINE_BIND_POINT_COMPUTE);
	vkCmdBindDescriptorSets(kCmdBuffer.Plat().GetCmdBuffer(), uiBindType,
		kMaterial.Plat().GetLayout(), uiBindPoint, 1, &m_pParamatersSet, 0, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// EOF
