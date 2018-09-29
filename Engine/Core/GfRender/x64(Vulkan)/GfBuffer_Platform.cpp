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

#include "GfRender/Common/GfBuffer.h"
#include "GfRender/Common/GfRenderContext.h"

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
// EOF