////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/16
//	File: 	GfCmdBufferFactory_Platform.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfCmdBufferFactory.h"
#include "GfRender/Common/GfRenderContext.h"

////////////////////////////////////////////////////////////////////////////////

GfCmdBufferFactory_Platform::GfCmdBufferFactory_Platform()
	: m_pPool(VK_NULL_HANDLE)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBufferFactory_Platform::InitPlatform(const GfRenderContext& kCtx)
{
	VkCommandPoolCreateInfo kInfo{};
	kInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	kInfo.pNext = nullptr;
	kInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
		VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
	kInfo.queueFamilyIndex = kCtx.m_uiPresentFamilyIndex;
	VkResult eResult = vkCreateCommandPool(kCtx.m_pDevice, &kInfo, nullptr, &m_pPool);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create command pool");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBufferFactory_Platform::CreateCmdBufferPlatform(const GfRenderContext& kCtx, u32 uiType, GfCmdBuffer& kOuCmdBuffer)
{
	bool bIsPrimary = (uiType == GfCmdBufferType::Primary);
	VkCommandBufferAllocateInfo kinfo;
	kinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	kinfo.pNext = nullptr;
	kinfo.commandPool = m_pPool;
	kinfo.level = bIsPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	kinfo.commandBufferCount = 1;

	VkCommandBuffer pCmdBuffer(nullptr);
	VkResult eResult = vkAllocateCommandBuffers(kCtx.m_pDevice, &kinfo, &pCmdBuffer);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to allocate command buffers");

	// Set the values to the Cmd buffer
	kOuCmdBuffer.Init((GfCmdBufferType::Type)uiType);
	kOuCmdBuffer.m_pCmdBuffer = pCmdBuffer;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
