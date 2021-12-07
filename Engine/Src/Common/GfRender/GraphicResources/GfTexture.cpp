////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/18
//	File: 	GfTexture.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GraphicResources/GfTexture.h"

////////////////////////////////////////////////////////////////////////////////

GfTextureView::GfTextureView(GfTexture* texture, const GfTextureViewConfig& config)
	: m_config(config)
	, m_texture(texture)
{}

GfTextureView::GfTextureView(GfTexture* texture)
	: m_config(texture->getDefaultViewConfig())
	, m_texture(texture)
{}

GfTextureView::GfTextureView() 
	: m_texture(nullptr)
{}

u64 GfTextureView::getViewID(const GfRenderContext& ctx)
{
	return m_texture->getViewIDForConfig(ctx, m_config);
}

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfTexture)
	, m_flags(0)
	, m_desc{}
	, m_defaultViewID(0)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture::externalInit(const GfRenderContext& ctx, const SwapchainDesc& kInitParams)
{
	if (!getIsInitialized())
	{
		m_desc.m_width = kInitParams.m_width;
		m_desc.m_height = kInitParams.m_height;
		m_desc.m_format = kInitParams.m_format;
		m_desc.m_mipCount = 1;
		m_desc.m_mappable = false;
		m_desc.m_usage = TextureUsageFlags::ColorAttachment; // See platform creation of swapchain
		m_defaultViewID = getDefaultViewID();
		m_kPlatform.externalInitPlat(ctx, kInitParams);
		m_flags |= InitializedAsSwapchain;
	}
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexture::create(const GfRenderContext& ctx, const TextureDesc& desc)
{
	if (!getIsInitialized())
	{
		m_desc = desc;
		m_kPlatform.create(ctx);
		bool isDepth = isDepthFormat(desc.m_format);
		if (!isDepth || (isDepth && (desc.m_usage & TextureUsageFlags::DepthStencil) != 0)) 
		{
			m_flags |= Flags::Initialized;
			m_defaultViewID = getDefaultViewID();
		}
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture::destroy(const GfRenderContext& ctx)
{
	m_kPlatform.destroy(ctx);
	m_flags = 0;
}

GfTextureViewConfig GfTexture::getDefaultViewConfig() const 
{
	GfTextureViewConfig view{};
	view.m_firstMipLevel = 0;
	view.m_mipLevelCount = getMipMapCount();
	view.m_firstSlice = 0;
	view.m_sliceCount = getSlices();
	return view;
}

GfTextureViewID GfTexture::getViewIDForConfig(const GfRenderContext& ctx, const struct GfTextureViewConfig& config) 
{
	if (config.getHash() == m_defaultViewID)
	{
		return m_kPlatform.getDefaultViewID();
	}
	return m_kPlatform.getViewIDForConfig(ctx, config);
}

u64 GfTexture::getDefaultViewID() 
{
	GfTextureViewConfig view = getDefaultViewConfig();
	return view.getHash();
}

////////////////////////////////////////////////////////////////////////////////

GfTexture2D::GfTexture2D() 
	: GfTexture()
{	
}

////////////////////////////////////////////////////////////////////////////////
// EOF