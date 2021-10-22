////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/04
//	File: 	GfSampler.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GraphicResources/GfSampler.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfSampler)
	, m_eMinFilter(ETexFilter::Linear)
	, m_eMagFilter(ETexFilter::Linear)
	, m_eMipMapMode(ESamplerMipMapMode::Linear)
	, m_eAddrU(ETexAddressMode::ClampToEdge)
	, m_eAddrV(ETexAddressMode::ClampToEdge)
	, m_eAddrW(ETexAddressMode::ClampToEdge)
	, m_fMipLodBias(0.0f)
	, m_fMinLod(0.0f)
	, m_fMaxLod(1.0f)
	, m_fMaxAnisotropy(1.0f)
	, m_bUseAnisotropy(false)
{
	m_eResourceType = ParamaterSlotType::Sampler;
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::Create(const GfRenderContext& kCtxt)
{
	if (!IsGPUReady()) 
	{
		if (!Plat().createRHI(kCtxt)) 
		{
			Destroy(kCtxt);
		}
		else 
		{
			MarkAsGPUReady();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::Destroy(const GfRenderContext& kCtxt)
{
	Plat().destroyRHI(kCtxt);
	MarkAsDestroyed();
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::SetMinFilter(ETexFilter::Type eFilter)
{
	if (!IsGPUReady()) 
	{
		m_eMinFilter = eFilter;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::SetMagFilter(ETexFilter::Type eFilter)
{
	if (!IsGPUReady()) 
	{
		m_eMagFilter = eFilter;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::SetMipMapMode(ESamplerMipMapMode::Type eMipMapMode)
{
	if (!IsGPUReady()) 
	{
		m_eMipMapMode = eMipMapMode;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::SetAddrU(ETexAddressMode::Type eAddrMode)
{
	if (!IsGPUReady()) 
	{
		m_eAddrU = eAddrMode;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::SetAddrV(ETexAddressMode::Type eAddrMode)
{
	if (!IsGPUReady()) 
	{
		m_eAddrV = eAddrMode;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::SetAddrW(ETexAddressMode::Type eAddrMode)
{
	if (!IsGPUReady()) 
	{
		m_eAddrW = eAddrMode;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::SetMipLodBias(f32 fBias)
{
	if (!IsGPUReady()) 
	{
		m_fMipLodBias = fBias;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::SetMinLod(f32 fMinLod)
{
	if (!IsGPUReady()) 
	{
		m_fMinLod = fMinLod;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::SetMaxLod(f32 fMaxLod)
{
	if (!IsGPUReady()) 
	{
		m_fMaxLod = fMaxLod;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::SetMaxAnisotropy(f32 fMaxAnisotropy)
{
	if (!IsGPUReady()) 
	{
		m_fMaxAnisotropy = fMaxAnisotropy;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::SetUseAnisotropy(bool bUseAnisotropy)
{
	if (!IsGPUReady()) 
	{
		m_bUseAnisotropy = bUseAnisotropy;
	}
}

////////////////////////////////////////////////////////////////////////////////

GfCombinedSamplerTexture::GfCombinedSamplerTexture()
	: m_pSampler(nullptr)
	, m_pTextureView(nullptr)
{
	m_eResourceType = ParamaterSlotType::CombinedImageSampler;
}

////////////////////////////////////////////////////////////////////////////////

void GfCombinedSamplerTexture::init(const GfSampler* pSampler, const GfTextureView* pView)
{
	if (!IsInitialised()) 
	{
		m_uiGraphicResFlags |= EGraphicResFlags::Initialised;
		m_pSampler = pSampler;
		m_pTextureView = pView;
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF