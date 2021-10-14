////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/04
//	File: 	GfSamplerState.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GraphicResources/GfSamplerState.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfSamplerState)
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

void GfSamplerState::Create(const GfRenderContext& kCtxt)
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

void GfSamplerState::Destroy(const GfRenderContext& kCtxt)
{
	Plat().destroyRHI(kCtxt);
	MarkAsDestroyed();
}

////////////////////////////////////////////////////////////////////////////////

void GfSamplerState::SetMinFilter(ETexFilter::Type eFilter)
{
	if (!IsGPUReady()) 
	{
		m_eMinFilter = eFilter;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSamplerState::SetMagFilter(ETexFilter::Type eFilter)
{
	if (!IsGPUReady()) 
	{
		m_eMagFilter = eFilter;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSamplerState::SetMipMapMode(ESamplerMipMapMode::Type eMipMapMode)
{
	if (!IsGPUReady()) 
	{
		m_eMipMapMode = eMipMapMode;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSamplerState::SetAddrU(ETexAddressMode::Type eAddrMode)
{
	if (!IsGPUReady()) 
	{
		m_eAddrU = eAddrMode;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSamplerState::SetAddrV(ETexAddressMode::Type eAddrMode)
{
	if (!IsGPUReady()) 
	{
		m_eAddrV = eAddrMode;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSamplerState::SetAddrW(ETexAddressMode::Type eAddrMode)
{
	if (!IsGPUReady()) 
	{
		m_eAddrW = eAddrMode;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSamplerState::SetMipLodBias(f32 fBias)
{
	if (!IsGPUReady()) 
	{
		m_fMipLodBias = fBias;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSamplerState::SetMinLod(f32 fMinLod)
{
	if (!IsGPUReady()) 
	{
		m_fMinLod = fMinLod;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSamplerState::SetMaxLod(f32 fMaxLod)
{
	if (!IsGPUReady()) 
	{
		m_fMaxLod = fMaxLod;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSamplerState::SetMaxAnisotropy(f32 fMaxAnisotropy)
{
	if (!IsGPUReady()) 
	{
		m_fMaxAnisotropy = fMaxAnisotropy;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfSamplerState::SetUseAnisotropy(bool bUseAnisotropy)
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

void GfCombinedSamplerTexture::init(const GfSamplerState* pSampler, const GfTextureView* pView)
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