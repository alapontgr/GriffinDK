////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/04
//	File: 	GfSamplerState_Platform.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GraphicResources/GfSampler.h"
#include "Common/GfRender/GfRenderContext.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfSampler)
	, m_sampler(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

bool GfSampler_Platform::create(const GfRenderContext& kCtxt)
{
	VkSamplerCreateInfo kSamplerInfo{};
	kSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	kSamplerInfo.flags = 0;
	kSamplerInfo.magFilter = ConvertTexFilter(m_kBase.m_desc.m_magFilter);
	kSamplerInfo.minFilter = ConvertTexFilter(m_kBase.m_desc.m_minFilter);
	kSamplerInfo.mipmapMode = ConvertTexMipMapMode(m_kBase.m_desc.m_mipMapMode);
	kSamplerInfo.addressModeU = ConvertTexAddressMode(m_kBase.m_desc.m_addrU);
	kSamplerInfo.addressModeV = ConvertTexAddressMode(m_kBase.m_desc.m_addrV);
	kSamplerInfo.addressModeW = ConvertTexAddressMode(m_kBase.m_desc.m_addrW);
	kSamplerInfo.mipLodBias = m_kBase.m_desc.m_mipLodBias;
	kSamplerInfo.anisotropyEnable = m_kBase.m_desc.m_useAnisotropy ? 1 : 0;
	kSamplerInfo.maxAnisotropy = m_kBase.m_desc.m_maxAnisotropy;
	kSamplerInfo.compareEnable = 0; // TODO: Needed?
	//kSamplerInfo.compareOp;
	kSamplerInfo.minLod = m_kBase.m_desc.m_minLod;
	kSamplerInfo.maxLod = m_kBase.m_desc.m_maxLod;
	kSamplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK; // TODO: Will I use border anytime?

	return vkCreateSampler(kCtxt.Plat().m_pDevice, &kSamplerInfo, nullptr, &m_sampler) == VK_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler_Platform::destroy(const GfRenderContext& kCtxt)
{
	if (m_sampler) 
	{
		vkDestroySampler(kCtxt.Plat().m_pDevice, m_sampler, nullptr);
		m_sampler = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF