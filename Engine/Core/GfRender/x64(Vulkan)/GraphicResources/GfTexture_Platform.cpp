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

#include "GfRender/Common/GraphicResources/GfTexture.h"
#include "GfRender/Common/GraphicResources/GfBuffer.h"
#include "GfRender/Common/GfRenderContext.h"
#include "GfRender/Common/GfCmdBuffer.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfTexturedResource)
, m_pImage(VK_NULL_HANDLE)
, m_uiAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
, m_pAlloc(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfTexturedResource_Platform::ExternalInitPlat(const GfExternTexInit_Platform& kInitParams)
{
	m_pImage = kInitParams.m_pExternalImage;
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexturedResource_Platform::CreateImageRHI(const GfRenderContext &kCtx, VkImageCreateInfo* pTextureInfo)
{
	if (vkCreateImage(kCtx.Plat().m_pDevice, pTextureInfo, nullptr, &m_pImage) != VK_SUCCESS)
	{
		return false;
	}
	// Assign aspect mask
	m_uiAspectMask = 0;
	if (m_kBase.IsDepthBuffer())
	{
		m_uiAspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	if (m_kBase.IsStencilBuffer())
	{
		m_uiAspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	if (!m_uiAspectMask)
	{
		m_uiAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexturedResource_Platform::AllocateImageMemoryRHI(const GfRenderContext &kCtx, VmaAllocationCreateInfo* pAllocInfo)
{
	if (vmaAllocateMemoryForImage(kCtx.Plat().m_kAllocator, m_pImage, pAllocInfo, &m_pAlloc, &m_kAllocInfo) != VK_SUCCESS)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexturedResource_Platform::BindImageWithMemoryRHI(const GfRenderContext &kCtx)
{
	if (vmaBindImageMemory(kCtx.Plat().m_kAllocator, m_pAlloc, m_pImage) != VK_SUCCESS)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void GfTexturedResource_Platform::DestroyRHI(const GfRenderContext &kCtx)
{
	if (m_pImage)
	{
		vkDestroyImage(kCtx.Plat().m_pDevice, m_pImage, nullptr);
		m_pImage = nullptr;

	}
	// Release allocated GPU memory
	if (m_pAlloc)
	{
		vmaFreeMemory(kCtx.Plat().m_kAllocator, m_pAlloc);
		m_pAlloc = nullptr;
		// Not really needed
		memset(&m_kAllocInfo, 0, sizeof(VmaAllocationInfo));
	}
}

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfTexture2D)
{
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexture2D_Platform::CreateRHI(const GfRenderContext& kCtx)
{
	return CreateImageRHI(kCtx);
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
	kTextureInfo.extent.height = m_kBase.m_uiHeight;
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

	
	if (m_kBase.GetSharedPlatform().CreateImageRHI(kCtx, &kTextureInfo))
	{
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

		if (m_kBase.GetSharedPlatform().AllocateImageMemoryRHI(kCtx, &kAllocInfo))
		{
			if (m_kBase.GetSharedPlatform().BindImageWithMemoryRHI(kCtx))
			{
				return true;
			}
		}
	}
	
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture2D_Platform::DestroyRHI(const GfRenderContext& kCtx)
{
	m_kBase.GetSharedPlatform().DestroyRHI(kCtx);
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture2D_Platform::LoadTexture2DDataFromStagingBufferRHI(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiBufferOffset)
{
	VkImage pImage(m_kBase.GetSharedPlatform().GetImage());
	// Transit image layout to transfer
	VkImageMemoryBarrier kBarrier{};
	kBarrier.image = pImage;
	kBarrier.subresourceRange.aspectMask = m_kBase.GetSharedPlatform().GetAspectMask();
	kBarrier.subresourceRange.baseArrayLayer = 0; // TODO: Give more flexibility
	kBarrier.subresourceRange.layerCount = 1; // TODO: Support for texture arrays
	kBarrier.subresourceRange.baseMipLevel = 0;
	kBarrier.subresourceRange.levelCount = m_kBase.GetMipMapCount();

	// From uninitialized -> Transfer
	kBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	kBarrier.srcAccessMask = 0;
	kBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	kBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	kBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	kBarrier.srcQueueFamilyIndex = kCtx.GetFamilyIdx(GfRenderContextFamilies::Graphics);
	kBarrier.dstQueueFamilyIndex = kCtx.GetFamilyIdx(GfRenderContextFamilies::Transfer);

	vkCmdPipelineBarrier(kCmdBuffer.Plat().GetCmdBuffer(),
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &kBarrier);

	// Copy buffer to image
	u32 uiRegions(m_kBase.GetMipMapCount());

	VkBufferImageCopy kRegion = {};
	kRegion.bufferOffset = uiBufferOffset;
	kRegion.bufferRowLength = 0;
	kRegion.bufferImageHeight = 0;

	kRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	kRegion.imageSubresource.mipLevel = 0;
	kRegion.imageSubresource.baseArrayLayer = 0;
	kRegion.imageSubresource.layerCount = 1;

	kRegion.imageOffset = { 0, 0, 0 }; // Offset X,Y,Z
	kRegion.imageExtent = {
		m_kBase.GetWidth(),
		m_kBase.GetHeight(),
		1
	};
	vkCmdCopyBufferToImage(kCmdBuffer.Plat().GetCmdBuffer(), kFrom.Plat().GetHandle(), pImage,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &kRegion);

	// Transit image layout to ready to be read by shaders
	// Transfer -> Ready for shader
	kBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	kBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	kBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	kBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	kBarrier.srcQueueFamilyIndex = kCtx.GetFamilyIdx(GfRenderContextFamilies::Transfer);
	kBarrier.dstQueueFamilyIndex = kCtx.GetFamilyIdx(GfRenderContextFamilies::Graphics);
	vkCmdPipelineBarrier(kCmdBuffer.Plat().GetCmdBuffer(),
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &kBarrier);
}

////////////////////////////////////////////////////////////////////////////////
// EOF