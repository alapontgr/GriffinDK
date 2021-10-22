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
	, m_pSampler(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////

bool GfSampler_Platform::createRHI(const GfRenderContext& kCtxt)
{
	VkSamplerCreateInfo kSamplerInfo{};
	kSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	kSamplerInfo.flags = 0;
	kSamplerInfo.magFilter = ConvertTexFilter(m_kBase.m_eMagFilter);
	kSamplerInfo.minFilter = ConvertTexFilter(m_kBase.m_eMinFilter);
	kSamplerInfo.mipmapMode = ConvertTexMipMapMode(m_kBase.m_eMipMapMode);
	kSamplerInfo.addressModeU = ConvertTexAddressMode(m_kBase.m_eAddrU);
	kSamplerInfo.addressModeV = ConvertTexAddressMode(m_kBase.m_eAddrV);
	kSamplerInfo.addressModeW = ConvertTexAddressMode(m_kBase.m_eAddrW);
	kSamplerInfo.mipLodBias = m_kBase.m_fMipLodBias;
	kSamplerInfo.anisotropyEnable = m_kBase.m_bUseAnisotropy ? 1 : 0;
	kSamplerInfo.maxAnisotropy = m_kBase.m_fMaxAnisotropy;
	kSamplerInfo.compareEnable = 0; // TODO: Needed?
	//kSamplerInfo.compareOp;
	kSamplerInfo.minLod = m_kBase.m_fMinLod;
	kSamplerInfo.maxLod = m_kBase.m_fMaxLod;
	kSamplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK; // TODO: Will I use border anytime?

	return vkCreateSampler(kCtxt.Plat().m_pDevice, &kSamplerInfo, nullptr, &m_pSampler) == VK_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler_Platform::destroyRHI(const GfRenderContext& kCtxt)
{
	if (m_pSampler) 
	{
		vkDestroySampler(kCtxt.Plat().m_pDevice, m_pSampler, nullptr);
		m_pSampler = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF