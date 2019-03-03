////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/18
//	File: 	GfTexture2D_Platform.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GraphicResources/GfTexture2D.h"
#include "GfRender/Common/GfRenderContext.h"

////////////////////////////////////////////////////////////////////////////////

GfTexture2D_Platform::GfTexture2D_Platform(GfTexture2D& kBase)
	: m_kBase(kBase)
	, m_pImage(nullptr)
	, m_pImageView(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexture2D_Platform::CreateRHI(const GfRenderContext& kCtx)
{
	if (CreateImageRHI(kCtx)) 
	{
		return CreateImageViewRHI(kCtx);
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexture2D_Platform::CreateImageRHI(const GfRenderContext &kCtx)
{
	VkImageCreateInfo kTextureInfo{};
	kTextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	kTextureInfo.mipLevels = m_kBase.m_uiMips;
	kTextureInfo.flags = 0; // TODO if needed
	kTextureInfo.imageType = VK_IMAGE_TYPE_2D;
	kTextureInfo.format = ConvertTextureFormat(m_kBase.m_eFormat);
	kTextureInfo.extent.width = m_kBase.m_uiWidth;
	kTextureInfo.extent.height = m_kBase.m_uiheight;
	kTextureInfo.extent.depth = 1;
	kTextureInfo.mipLevels = m_kBase.m_uiMips;
	kTextureInfo.arrayLayers = 1; // TODO: Add support for arrays
	kTextureInfo.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Add support for MS
	kTextureInfo.tiling = m_kBase.IsTilable() ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;
	kTextureInfo.usage = ConvertTextureUsageMask(m_kBase.m_uiUsage);
	kTextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Add support for concurrent usage
	kTextureInfo.queueFamilyIndexCount = 0;
	kTextureInfo.pQueueFamilyIndices = nullptr;
	kTextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (vkCreateImage(kCtx.m_pDevice, &kTextureInfo, nullptr, &m_pImage) != VK_SUCCESS)
	{
		return false;
	}

	// Allocate memory and bind it
	// Use vmaAllocateMemoryForImage
	VmaAllocationCreateInfo kAllocInfo{};
	kAllocInfo.flags = VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT;
	kAllocInfo.usage = m_kBase.IsMappable() ? VMA_MEMORY_USAGE_CPU_TO_GPU : VMA_MEMORY_USAGE_GPU_ONLY;
	kAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // Is this needed?
	kAllocInfo.preferredFlags = 0;
	kAllocInfo.memoryTypeBits = 0;
	kAllocInfo.pool = VK_NULL_HANDLE; // TODO: Use pool for textures
	kAllocInfo.pUserData = nullptr;

	if (vmaAllocateMemoryForImage(kCtx.m_kAllocator, m_pImage, &kAllocInfo, &m_pAlloc, &m_kAllocInfo) != VK_SUCCESS)
	{
		return false;
	}

	if (vmaBindImageMemory(kCtx.m_kAllocator, m_pAlloc, m_pImage) != VK_SUCCESS)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexture2D_Platform::CreateImageViewRHI(const GfRenderContext &kCtx)
{
	VkImageViewCreateInfo kImageViewInfo{};
	kImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	kImageViewInfo.flags = 0; // TODO?
	kImageViewInfo.image = m_pImage;
	kImageViewInfo.format = ConvertTextureFormat(m_kBase.m_eFormat);
	//kImageViewInfo.components = 0; // Default value of 0, equivelant to identity
	VkImageAspectFlags uiAspect(0);
	if (m_kBase.IsDepthBuffer()) 
	{
		uiAspect |= VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	if (m_kBase.IsStencilBuffer())
	{
		uiAspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	if (!uiAspect) 
	{
		uiAspect = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	kImageViewInfo.subresourceRange.aspectMask = uiAspect;
	kImageViewInfo.subresourceRange.baseMipLevel = 0;	// TODO: Any cases where we may want to limit the mips?
	kImageViewInfo.subresourceRange.levelCount = m_kBase.m_uiMips;
	kImageViewInfo.subresourceRange.baseArrayLayer = 0; // TODO: Support for arrays of texture-arrays
	kImageViewInfo.subresourceRange.layerCount = 1;
	return vkCreateImageView(kCtx.m_pDevice, &kImageViewInfo, nullptr, &m_pImageView) != VK_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture2D_Platform::DestroyRHI(const GfRenderContext& kCtx)
{
	if (m_pImageView) 
	{
		vkDestroyImageView(kCtx.m_pDevice, m_pImageView, nullptr);
		m_pImageView = nullptr;
	}
	if (m_pImage) 
	{
		vkDestroyImage(kCtx.m_pDevice, m_pImage, nullptr);
		m_pImage = nullptr;
	
	}
	// Release allocated GPU memory
	if (m_pAlloc) 
	{
		vmaFreeMemory(kCtx.m_kAllocator, m_pAlloc);
		m_pAlloc = nullptr;
		// Not really needed
		memset(&m_kAllocInfo, 0, sizeof(VmaAllocationInfo));
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF