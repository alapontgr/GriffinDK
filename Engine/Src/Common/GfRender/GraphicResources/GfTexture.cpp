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

GF_DEFINE_BASE_CTOR(GfTexture)
	, m_flags(0)
	, m_desc{}
{
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexture::init(const TextureDesc& desc)
{
	if (!getIsInitialized())
	{
		bool isDepth = isDepthFormat(desc.m_format);
		if (!isDepth || (isDepth && (desc.m_usage & TextureUsage::DepthStencil) != 0)) 
		{
			m_desc = desc;
			m_flags |= Flags::Initialized;
			m_kPlatform.init(m_desc);
			return true;
		}
	}
	return false;
}

void GfTexture::ExternalInit(const SwapchainDesc& kInitParams)
{
	if (!getIsInitialized())
	{
		m_desc.m_width = kInitParams.m_width;
		m_desc.m_height = kInitParams.m_height;
		m_desc.m_format = kInitParams.m_format;
		m_kPlatform.ExternalInitPlat(kInitParams);
		m_flags |= InitializedAsSwapchain;
		m_flags |= Flags::GPUReady;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture::Create(const GfRenderContext& kCtx)
{
	if (getIsInitialized()) 
	{
		if (!m_kPlatform.createRHI(kCtx))
		{
			m_kPlatform.destroyRHI(kCtx);
		}
		else 
		{
			m_flags |= Flags::GPUReady;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfTexture::Destroy(const GfRenderContext& kCtx)
{
	// Destroy only if needed. Do not worry if the texture was initialized with an external source
	if (getIsGPUReady() && !getIsSwapchain())
	{
		m_kPlatform.destroyRHI(kCtx);
		m_flags = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////

GfTexture2D::GfTexture2D() 
	: GfTexture()
{	
}

////////////////////////////////////////////////////////////////////////////////

bool GfTexture2D::init(u32 width, u32 height, u32 mips,
	TextureFormat::Type format)
{
	TextureDesc desc;
	desc.m_width = width;
	desc.m_height = height;
	desc.m_depth = 1;
	desc.m_slices = 1;
	desc.m_mipCount = mips;
	desc.m_textureType = TextureType::Type_2D;
	desc.m_format = format;
	desc.m_usage = TextureUsage::Sample;
	return GfTexture::init(desc);
}

////////////////////////////////////////////////////////////////////////////////
// EOF