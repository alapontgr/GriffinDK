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

#include "Common/GfRender/GraphicResources/GfTexture.h"
#include "Common/GfRender/GraphicResources/GfBuffer.h"
#include "Common/GfRender/GfRenderContext.h"
#include "Common/GfRender/GfCmdBuffer.h"

////////////////////////////////////////////////////////////////////////////////

VkImageType getImageType(TextureType type) 
{
	VkImageType s_VulkanTextureTypes[] =
	{
		VK_IMAGE_TYPE_1D,
		VK_IMAGE_TYPE_2D,
		VK_IMAGE_TYPE_3D,
	};
	return s_VulkanTextureTypes[static_cast<u32>(type)];
}

VkImageUsageFlags convertToVulkanUsage(TextureType type, TextureUsageMask usage) 
{
	VkImageUsageFlags vkUsage = 0;

	vkUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	if ((usage & TextureUsage::Sample) != 0) 
	{
		vkUsage |= VK_IMAGE_USAGE_SAMPLED_BIT;
	}

	if ((usage & TextureUsage::Storage) != 0) 
	{
		vkUsage |= VK_IMAGE_USAGE_STORAGE_BIT;
	}

	if ((usage & TextureUsage::RenderTarget) != 0) 
	{
		vkUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	}

	if ((usage & TextureUsage::DepthStencil) != 0) 
	{
		vkUsage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
	}

	return vkUsage;
}

VkImageViewType convertToImageViewType(TextureType type, u32 w, u32 h, u32 d, u32 slices) 
{
	VkImageViewType viewT = VK_IMAGE_VIEW_TYPE_2D;
	// TODO CUBEMAP support
	switch (type) 
	{
	case TextureType::Type_1D:
		viewT = slices == 1 ? VK_IMAGE_VIEW_TYPE_1D : VK_IMAGE_VIEW_TYPE_1D_ARRAY;
		break;
	case TextureType::Type_2D:
		viewT = slices == 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY ;
		break;
	case TextureType::Type_3D:
		GF_ASSERT(d > 1, "Invalid depth for 3D Texture");
		viewT = VK_IMAGE_VIEW_TYPE_3D;
		break;
	default:
		break;
	}
	return viewT;
}

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfTexture)
, m_currLayout(VK_IMAGE_LAYOUT_UNDEFINED)
, m_image(VK_NULL_HANDLE)
, m_view(nullptr)
, m_uiAspectMask(VK_IMAGE_ASPECT_COLOR_BIT)
, m_pAlloc(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture_Platform::init(const TextureDesc& desc)
{
}

void GfTexture_Platform::ExternalInitPlat(const GfExternTexInit_Platform& kInitParams)
{
	m_image = kInitParams.m_pExternalImage;
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexture_Platform::createRHI(const GfRenderContext &kCtx)
{
	VkImageCreateInfo kTextureInfo{};
	kTextureInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	kTextureInfo.mipLevels = m_kBase.m_desc.m_mipCount;
	kTextureInfo.flags = 0; // TODO if needed
	kTextureInfo.imageType = getImageType(m_kBase.m_desc.m_textureType);
	kTextureInfo.format = ConvertTextureFormat(m_kBase.m_desc.m_format);
	kTextureInfo.extent.width = m_kBase.m_desc.m_width;
	kTextureInfo.extent.height = m_kBase.m_desc.m_height;
	kTextureInfo.extent.depth = m_kBase.m_desc.m_depth;
	kTextureInfo.arrayLayers = m_kBase.m_desc.m_slices;
	kTextureInfo.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: Add support for MS
	kTextureInfo.tiling = m_kBase.getIsMappable() ? VK_IMAGE_TILING_LINEAR : VK_IMAGE_TILING_OPTIMAL;
	kTextureInfo.usage = convertToVulkanUsage(m_kBase.m_desc.m_textureType, m_kBase.m_desc.m_usage);
	kTextureInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: Add support for concurrent usage
	kTextureInfo.queueFamilyIndexCount = 0;
	kTextureInfo.pQueueFamilyIndices = nullptr;
	kTextureInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	
	if (vkCreateImage(kCtx.Plat().m_pDevice, &kTextureInfo, nullptr, &m_image) != VK_SUCCESS)
	{
		return false;
	}

	// Assign aspect mask
	m_uiAspectMask = 0;
	if (isDepthFormat(m_kBase.m_desc.m_format))
	{
		m_uiAspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	if (isStencilFormat(m_kBase.m_desc.m_format))
	{
		m_uiAspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	if (!m_uiAspectMask)
	{
		m_uiAspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	// Allocate memory and bind it
	// Use vmaAllocateMemoryForImage
	VmaAllocationCreateInfo kAllocInfo{};
	kAllocInfo.flags = VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT;
	kAllocInfo.usage = m_kBase.getIsMappable() ? VMA_MEMORY_USAGE_CPU_TO_GPU : VMA_MEMORY_USAGE_GPU_ONLY;
	kAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // Is this needed?
	kAllocInfo.preferredFlags = 0;
	kAllocInfo.memoryTypeBits = 0;
	kAllocInfo.pool = VK_NULL_HANDLE; // TODO: Use pool for textures
	kAllocInfo.pUserData = nullptr;

	if (allocateImageMemoryRHI(kCtx, &kAllocInfo))
	{
		if (bindImageWithMemoryRHI(kCtx))
		{
			createView(kCtx);
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexture_Platform::allocateImageMemoryRHI(const GfRenderContext &kCtx, VmaAllocationCreateInfo* pAllocInfo)
{
	if (vmaAllocateMemoryForImage(kCtx.Plat().m_kAllocator, m_image, pAllocInfo, &m_pAlloc, &m_kAllocInfo) != VK_SUCCESS)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexture_Platform::bindImageWithMemoryRHI(const GfRenderContext &kCtx)
{
	if (vmaBindImageMemory(kCtx.Plat().m_kAllocator, m_pAlloc, m_image) != VK_SUCCESS)
	{
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture_Platform::destroyRHI(const GfRenderContext &kCtx)
{
	if (m_image)
	{
		vkDestroyImage(kCtx.Plat().m_pDevice, m_image, nullptr);
		m_image = nullptr;

	}
	// Release allocated GPU memory
	if (m_pAlloc)
	{
		vmaFreeMemory(kCtx.Plat().m_kAllocator, m_pAlloc);
		m_pAlloc = nullptr;
		// Not really needed
		memset(&m_kAllocInfo, 0, sizeof(VmaAllocationInfo));
	}

	if (m_view) 
	{
		vkDestroyImageView(kCtx.Plat().m_pDevice, m_view, nullptr);
		m_view = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture_Platform::loadTexture2DDataFromStagingBufferRHI(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiBufferOffset)
{
	VkImage pImage(getImage());
	// Transit image layout to transfer
	VkImageMemoryBarrier kBarrier{};
	kBarrier.image = pImage;
	kBarrier.subresourceRange.aspectMask = getAspectMask();
	kBarrier.subresourceRange.baseArrayLayer = 0; // TODO: Give more flexibility
	kBarrier.subresourceRange.layerCount = 1; // TODO: Support for texture arrays
	kBarrier.subresourceRange.baseMipLevel = 0;
	kBarrier.subresourceRange.levelCount = m_kBase.getMipMapCount();

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
	u32 uiRegions(m_kBase.getMipMapCount());

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
		m_kBase.getWidth(),
		m_kBase.getHeight(),
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

void GfTexture_Platform::createView(const GfRenderContext& ctx)
{
	VkImageViewCreateInfo  viewCI{};
	viewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCI.pNext = nullptr;
	viewCI.flags = 0;
	viewCI.image = m_image;
	viewCI.viewType = convertToImageViewType(
		m_kBase.getTextureType(),
		m_kBase.getWidth(),
		m_kBase.getHeight(),
		m_kBase.getDepth(),
		m_kBase.getSlices());
	viewCI.format = ConvertTextureFormat(m_kBase.getFormat());
	viewCI.components = {
		VK_COMPONENT_SWIZZLE_IDENTITY,		// R
		VK_COMPONENT_SWIZZLE_IDENTITY,		// G
		VK_COMPONENT_SWIZZLE_IDENTITY,		// B
		VK_COMPONENT_SWIZZLE_IDENTITY };	// A
	viewCI.subresourceRange.aspectMask = m_uiAspectMask;
	viewCI.subresourceRange.baseMipLevel = 0;
	viewCI.subresourceRange.levelCount = m_kBase.getMipMapCount();
	viewCI.subresourceRange.baseArrayLayer = 0;
	viewCI.subresourceRange.layerCount = m_kBase.getSlices();
	if (!vkCreateImageView(ctx.Plat().m_pDevice, &viewCI, nullptr, &m_view)) 
	{
		GF_ERROR("Failed to create image view");
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF