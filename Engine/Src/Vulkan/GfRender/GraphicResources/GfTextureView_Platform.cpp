////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/21
//	File: 	GfTextureView_Platform.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GraphicResources/GfTextureView.h"
#include "Common/GfRender/GfRenderContext.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfTextureView) 
	, m_pImageView(nullptr)
{	
}

////////////////////////////////////////////////////////////////////////////////

void GfTextureView_Platform::ExternInitPlat(const GfExternTexView_Platform& kInitParams)
{
	m_pImageView = kInitParams.m_pView;
}

////////////////////////////////////////////////////////////////////////////////

bool GfTextureView_Platform::CreateRHI(const GfRenderContext& kCtx)
{
 	const GfTexturedResource* pTexture(m_kBase.m_pTexture);

 	VkImageViewCreateInfo kImageViewInfo{};
 	kImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
 	kImageViewInfo.flags = 0; // TODO?
 	kImageViewInfo.viewType = ConvertViewType(m_kBase.m_eViewType);
 	kImageViewInfo.format = ConvertTextureFormat(pTexture->GetFormat());
 	//kImageViewInfo.components = 0; // Default value of 0, equivelant to identity
 	kImageViewInfo.subresourceRange.aspectMask = pTexture->GetSharedPlatformC().GetAspectMask();
 	kImageViewInfo.subresourceRange.baseMipLevel = m_kBase.m_uiBaseLod;
 	kImageViewInfo.subresourceRange.levelCount = m_kBase.m_uiLodCount;
	kImageViewInfo.subresourceRange.baseArrayLayer = m_kBase.m_uiBaseLayerIdx;
 	kImageViewInfo.subresourceRange.layerCount = m_kBase.m_uiLayerCount;
	kImageViewInfo.image = pTexture->GetSharedPlatformC().GetImage();
	return vkCreateImageView(kCtx.Plat().m_pDevice, &kImageViewInfo, nullptr, &m_pImageView) == VK_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

void GfTextureView_Platform::DestroyRHI(const GfRenderContext& kCtx)
{
	if (m_pImageView) 
	{
		vkDestroyImageView(kCtx.Plat().m_pDevice, m_pImageView, nullptr);
		m_pImageView = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF