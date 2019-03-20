////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/29
//	File: 	GfBuffer_Platform.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GraphicResources/GfBuffer.h"
#include "GfRender/Common/GfRenderContext.h"
#include "GfRender/Common/GfCmdBuffer.h"

#include "GfCore/Common/GfStl.h"

////////////////////////////////////////////////////////////////////////////////
// Utils

// Used to obtain the StageFlags and AccessFlags to be used by a PipelineBarrier to perform the transition to a specific EBufferUsage
static const GfUMap<EBufferUsage::Type, GfStageAccessConfig> g_tVulkanStageAccessFlagsMap {
	{ EBufferUsage::Transfer_Dst,	{ VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT } },
	{ EBufferUsage::Transfer_Dst,	{ VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT } },
	{ EBufferUsage::Index_Buffer,	{ VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_INDEX_READ_BIT } },
	{ EBufferUsage::Vertex_Buffer,	{ VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT } },
	{ EBufferUsage::Uniform_Buffer,	{ VK_PIPELINE_STAGE_VERTEX_SHADER_BIT , VK_ACCESS_UNIFORM_READ_BIT } }
};

////////////////////////////////////////////////////////////////////////////////
// Functions

u32 FindMemTypeIdx(const u32 uiTypeFilter, VkMemoryPropertyFlags uiMemProperties, VkPhysicalDevice pPhysicalDevice)
{
	VkPhysicalDeviceMemoryProperties deviceMemProperties{};
	vkGetPhysicalDeviceMemoryProperties(pPhysicalDevice, &deviceMemProperties);
	for (u32 i = 0; i < deviceMemProperties.memoryTypeCount; ++i)
	{
		if ((uiTypeFilter & (1 << i)) && (deviceMemProperties.memoryTypes[i].propertyFlags & uiMemProperties) == uiMemProperties)
		{
			return i;
		}
	}
	GF_ASSERT_ALWAYS("Failed to find a type of memory with the given filter and properties");
	return 0xffffffff;
}

////////////////////////////////////////////////////////////////////////////////

GfStageAccessConfig GfBuffer_Platform::GetTransitionSettingsForType(u32 uiType)
{
	return g_tVulkanStageAccessFlagsMap.at((EBufferUsage::Type)uiType);
}

////////////////////////////////////////////////////////////////////////////////

GfBuffer_Platform::GfBuffer_Platform(GfBuffer& kBase)
	: m_kBase(kBase)
	, m_pBuffer(nullptr)
	, m_pMemory(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

bool GfBuffer_Platform::InitPlatform(const GfRenderContext& kCtxt)
{
	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0; // TODO: Add support for different flags (SParse buffers)
	bufferCreateInfo.size = m_kBase.m_kDesc.m_ulSize;
	bufferCreateInfo.usage = m_kBase.m_kDesc.m_mBufferUsage.Flags();

	// TODO: Enable Optional concurrent access at some point
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = nullptr;

	VkResult eResult = vkCreateBuffer(kCtxt.m_pDevice, &bufferCreateInfo, nullptr, &m_pBuffer);
	if (eResult != VK_SUCCESS) 
	{
		return false;
	}

	// Get the requirements
	VkMemoryRequirements bufferRequirements{};
	vkGetBufferMemoryRequirements(kCtxt.m_pDevice, m_pBuffer, &bufferRequirements);

	// Allocate the memory
	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.allocationSize = bufferRequirements.size;
	allocateInfo.memoryTypeIndex = FindMemTypeIdx(
		bufferRequirements.memoryTypeBits, 
		m_kBase.m_kDesc.m_mMemoryProperties.Flags(), 
		kCtxt.m_pPhysicalDevice);

	eResult = vkAllocateMemory(kCtxt.m_pDevice, &allocateInfo, nullptr, &m_pMemory);
	if (eResult != VK_SUCCESS)
	{
		return false;
	}

	// Bind it to the buffer
	vkBindBufferMemory(kCtxt.m_pDevice, m_pBuffer, m_pMemory, 0);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer_Platform::DestroyPlatform(const GfRenderContext& kCtxt)
{
	if (m_pBuffer) 
	{
		vkDestroyBuffer(kCtxt.m_pDevice, m_pBuffer, nullptr);
		vkFreeMemory(kCtxt.m_pDevice, m_pMemory, nullptr);
		m_pBuffer = nullptr;
		m_pMemory = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

GfStageAccessConfig GfBuffer_Platform::GetTransitionSettings() const
{
	return g_tVulkanStageAccessFlagsMap.at(m_kBase.m_kDesc.m_eBufferType);
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer_Platform::CopyRangeRHI(
	const GfCmdBuffer& kCmdBuffer,
	const GfBuffer& kFrom,
	const GfBuffer& kTo,
	u32 uiFromOffset, u32 uiToOffset, u32 uiSize)
{
	GfStageAccessConfig kCurrToTransferSettings = kTo.GetTransitionSettings();
	GfStageAccessConfig kTransferToCurrSettings = GfBuffer_Platform::GetTransitionSettingsForType(EBufferUsage::Transfer_Dst);

	VkBufferMemoryBarrier kBarrier;
	kBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	kBarrier.pNext = nullptr;
	kBarrier.buffer = kTo.GetHandle();
	kBarrier.offset = uiToOffset;
	kBarrier.size = uiSize;
	kBarrier.srcAccessMask = kCurrToTransferSettings.m_eAccess;
	kBarrier.dstAccessMask = kTransferToCurrSettings.m_eAccess;
	// TODO: Do I need to transit between different queues
	kBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	kBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	vkCmdPipelineBarrier(
		kCmdBuffer.GetCmdBuffer(),
		kCurrToTransferSettings.m_eStage,
		kTransferToCurrSettings.m_eStage,
		0,
		0, nullptr,
		1, &kBarrier,
		0, nullptr);

	VkBufferCopy kRegion;
	kRegion.srcOffset = uiFromOffset;
	kRegion.dstOffset = uiToOffset;
	kRegion.size = uiSize;
	vkCmdCopyBuffer(kCmdBuffer.GetCmdBuffer(), kFrom.GetHandle(), kTo.GetHandle(), 1, &kRegion);

	// Go back to the original state
	kBarrier.srcAccessMask = kTransferToCurrSettings.m_eAccess;
	kBarrier.dstAccessMask = kCurrToTransferSettings.m_eAccess;
	vkCmdPipelineBarrier(
		kCmdBuffer.GetCmdBuffer(),
		kTransferToCurrSettings.m_eStage,
		kCurrToTransferSettings.m_eStage,
		0,
		0, nullptr,
		1, &kBarrier,
		0, nullptr);
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer_Platform::UpdateRangeRHI(
	const GfCmdBuffer& kCmdBuffer,
	const GfBuffer& kBuffer,
	u32 uiOffset, u32 uiSize, void* pData)
{
	GfStageAccessConfig kCurrToTransferSettings = kBuffer.GetTransitionSettings();
	GfStageAccessConfig kTransferToCurrSettings = GfBuffer_Platform::GetTransitionSettingsForType(EBufferUsage::Transfer_Dst);

	// Sync to transfer operations
	VkBufferMemoryBarrier barrier;
	barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	barrier.pNext = nullptr;
	barrier.buffer = kBuffer.GetHandle();
	barrier.offset = uiOffset;
	barrier.size = uiSize;
	barrier.srcAccessMask = kCurrToTransferSettings.m_eAccess;
	barrier.dstAccessMask = kTransferToCurrSettings.m_eAccess;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	vkCmdPipelineBarrier(
		kCmdBuffer.GetCmdBuffer(),
		kCurrToTransferSettings.m_eStage,
		kTransferToCurrSettings.m_eStage,
		0,
		0, nullptr,
		1, &barrier,
		0, nullptr);

	// Update data
	vkCmdUpdateBuffer(kCmdBuffer.GetCmdBuffer(), kBuffer.GetHandle(), uiOffset, uiSize, pData);

	// Sync back
	barrier.srcAccessMask = kTransferToCurrSettings.m_eAccess;
	barrier.dstAccessMask = kCurrToTransferSettings.m_eAccess;
	vkCmdPipelineBarrier(
		kCmdBuffer.GetCmdBuffer(),
		kTransferToCurrSettings.m_eStage,
		kCurrToTransferSettings.m_eStage,
		0,
		0, nullptr,
		1, &barrier,
		0, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// EOF