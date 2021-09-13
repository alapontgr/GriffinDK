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

#include "Common/GfRender/GraphicResources/GfBuffer.h"
#include "Common/GfRender/GfRenderContext.h"
#include "Common/GfRender/GfCmdBuffer.h"

#include "Common/GfCore/GfStl.h"

////////////////////////////////////////////////////////////////////////////////
// Utils

struct GfStageAccessConfig
{
	VkPipelineStageFlags	m_stage;
	VkAccessFlags			m_access;
};

static const GfStageAccessConfig s_bufferTypePreferredSyncState[] =
{
	{VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT }, // Staging
	{VK_PIPELINE_STAGE_VERTEX_INPUT_BIT , VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT }, // Vertex buffer
	{VK_PIPELINE_STAGE_VERTEX_INPUT_BIT , VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT }, // Index buffer
	{VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT , VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT }, // Uniform buffer
	{VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT , VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT }, // Storage buffer
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

static void getBufferFlags(BufferType::Type type, bool isMappable, VkBufferUsageFlags& usageFlags, VkMemoryPropertyFlags& memoryFlags)
{
	usageFlags = 0;
	memoryFlags = 0;
	if (isMappable) 
	{
		memoryFlags |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	}
	else 
	{
		memoryFlags |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	}
	switch (type)
	{
	case BufferType::Vertex:
		usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case BufferType::Index:
		usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case BufferType::Uniform:
		usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		memoryFlags |= isMappable ? 0 : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case BufferType::Staging:
		usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		break;
	default:
		GF_ASSERT_ALWAYS("Buffer type not supported");
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////

GfStageAccessConfig getTransitionSettingsForType(u32 bufferType)
{
	return s_bufferTypePreferredSyncState[bufferType];
}

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfBuffer)
	, m_pBuffer(nullptr)
	, m_pMemory(nullptr)
	, m_currAccess(0)
	, m_currStage(0)
{
}

////////////////////////////////////////////////////////////////////////////////

bool GfBuffer_Platform::createRHI(const GfRenderContext& kCtxt)
{
	VkBufferUsageFlags usageFlags = 0;
	VkMemoryPropertyFlags memoryFlags = 0;
	getBufferFlags(m_kBase.m_desc.m_bufferType, m_kBase.m_desc.m_mappable, usageFlags, memoryFlags);

	VkBufferCreateInfo bufferCreateInfo{};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0; // TODO: Add support for different flags (SParse buffers)
	bufferCreateInfo.size = static_cast<VkDeviceSize>(m_kBase.m_desc.m_size);
	bufferCreateInfo.usage = usageFlags;

	// TODO: Enable Optional concurrent access at some point
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = nullptr;

	VkResult eResult = vkCreateBuffer(kCtxt.Plat().m_pDevice, &bufferCreateInfo, nullptr, &m_pBuffer);
	if (eResult != VK_SUCCESS) 
	{
		return false;
	}

	// Get the requirements
	VkMemoryRequirements bufferRequirements{};
	vkGetBufferMemoryRequirements(kCtxt.Plat().m_pDevice, m_pBuffer, &bufferRequirements);

	// Allocate the memory
	VkMemoryAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.allocationSize = bufferRequirements.size;
	allocateInfo.memoryTypeIndex = FindMemTypeIdx(bufferRequirements.memoryTypeBits, memoryFlags, kCtxt.Plat().m_pPhysicalDevice);

	eResult = vkAllocateMemory(kCtxt.Plat().m_pDevice, &allocateInfo, nullptr, &m_pMemory);
	if (eResult != VK_SUCCESS)
	{
		return false;
	}

	// Bind it to the buffer
	vkBindBufferMemory(kCtxt.Plat().m_pDevice, m_pBuffer, m_pMemory, 0);
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer_Platform::destroyRHI(const GfRenderContext& kCtxt)
{
	if (m_pBuffer) 
	{
		vkDestroyBuffer(kCtxt.Plat().m_pDevice, m_pBuffer, nullptr);
		vkFreeMemory(kCtxt.Plat().m_pDevice, m_pMemory, nullptr);
		m_pBuffer = nullptr;
		m_pMemory = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

void* GfBuffer_Platform::MapRHI(const GfRenderContext& kCtxt, u32 uiOffset, u32 uiSize)
{
	void* pData(nullptr);
	VkResult siResult =  vkMapMemory(kCtxt.Plat().m_pDevice, m_pMemory, uiOffset, uiSize, 0, &pData);
	GF_ASSERT(siResult == VK_SUCCESS, "Failed to Map buffer's memory");
	return pData;
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer_Platform::unMapRHI(const GfRenderContext& kCtxt)
{
	vkUnmapMemory(kCtxt.Plat().m_pDevice, m_pMemory);
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer_Platform::flushAndUnMapRHI(const GfRenderContext& kCtxt, u32 uiOffset, u32 uiSize)
{
	VkMappedMemoryRange kRange = {};
	kRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	kRange.memory = m_pMemory;
	kRange.offset = uiOffset;
	kRange.size = uiSize;
	vkFlushMappedMemoryRanges(kCtxt.Plat().m_pDevice, 1, &kRange);
	vkUnmapMemory(kCtxt.Plat().m_pDevice, m_pMemory);
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer_Platform::copyRangeFromRHI(
	const GfCmdBuffer& kCmdBuffer,
	const GfBuffer& kFrom,
	u32 uiFromOffset, u32 uiToOffset, u32 uiSize)
{
	GF_ASSERT(kFrom.m_kPlatform.m_currAccess == 0 && kFrom.m_kPlatform.m_currStage == 0, "The source has not been transited to a correct state");
	GfStageAccessConfig srcSettings = getTransitionSettingsForType(kFrom.m_desc.m_bufferType);
	GfStageAccessConfig dstSettings = getTransitionSettingsForType(m_kBase.m_desc.m_bufferType);

	VkBufferMemoryBarrier barriers[2];
	// From buffer 
	barriers[0].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	barriers[0].pNext = nullptr;
	barriers[0].buffer = kFrom.m_kPlatform.GetHandle();
	barriers[0].offset = uiFromOffset;
	barriers[0].size = uiSize;
	barriers[0].srcAccessMask = srcSettings.m_access;
	barriers[0].dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	// Destination buffer (Me)
	barriers[1].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	barriers[1].pNext = nullptr;
	barriers[1].buffer = m_pBuffer;
	barriers[1].offset = uiToOffset;
	barriers[1].size = uiSize;
	barriers[1].srcAccessMask = dstSettings.m_access;
	barriers[1].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	// Sync with any compute or fragment operations in fly
	vkCmdPipelineBarrier(
		kCmdBuffer.Plat().getCmdBuffer(),
		srcSettings.m_stage | dstSettings.m_stage,
		VK_PIPELINE_STAGE_TRANSFER_BIT ,
		0,
		0, nullptr,
		2, barriers,
		0, nullptr);

	VkBufferCopy kRegion;
	kRegion.srcOffset = uiFromOffset;
	kRegion.dstOffset = uiToOffset;
	kRegion.size = uiSize;
	vkCmdCopyBuffer(kCmdBuffer.Plat().getCmdBuffer(), kFrom.Plat().GetHandle(), m_pBuffer, 1, &kRegion);

	// Go back to the original state and sync with Compute and vertex
	barriers[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barriers[0].dstAccessMask = dstSettings.m_access;
	barriers[1].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barriers[1].dstAccessMask = dstSettings.m_access;

	vkCmdPipelineBarrier(
		kCmdBuffer.Plat().getCmdBuffer(),
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		srcSettings.m_stage | dstSettings.m_stage,
		0,
		0, nullptr,
		2, barriers,
		0, nullptr);

	if (m_currAccess == 0 && m_currStage == 0) 
	{
		m_currAccess = dstSettings.m_access;
		m_currStage = dstSettings.m_stage;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfBuffer_Platform::updateRangeRHI(
	const GfCmdBuffer& kCmdBuffer,
	u32 uiOffset, u32 uiSize, void* pData)
{
	GfStageAccessConfig syncSettings = getTransitionSettingsForType(m_kBase.m_desc.m_bufferType);

	// Sync to transfer operations
	VkBufferMemoryBarrier barrier;
	barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	barrier.pNext = nullptr;
	barrier.buffer = m_pBuffer;
	barrier.offset = uiOffset;
	barrier.size = uiSize;
	barrier.srcAccessMask = syncSettings.m_access;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	vkCmdPipelineBarrier(
		kCmdBuffer.Plat().getCmdBuffer(),
		syncSettings.m_stage,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		1, &barrier,
		0, nullptr);

	// Update data
	vkCmdUpdateBuffer(kCmdBuffer.Plat().getCmdBuffer(), m_pBuffer, uiOffset, uiSize, pData);

	// Sync back
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = syncSettings.m_access;
	vkCmdPipelineBarrier(
		kCmdBuffer.Plat().getCmdBuffer(),
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		syncSettings.m_stage,
		0,
		0, nullptr,
		1, &barrier,
		0, nullptr);

	if (m_currAccess == 0 && m_currStage == 0) 
	{
		m_currAccess = syncSettings.m_access;
		m_currStage = syncSettings.m_stage;
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF