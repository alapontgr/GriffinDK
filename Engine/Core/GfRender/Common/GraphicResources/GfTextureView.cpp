////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/21
//	File: 	GfTextureView.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GraphicResources/GfTextureView.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfTextureView) 
	, m_pTexture(nullptr)
	, m_eViewType(ETextureViewType::View2D)
	, m_uiBaseLod(0)
	, m_uiLodCount(1)
	, m_uiBaseLayerIdx(0)
	, m_uiLayerCount(1)
{
	m_eResourceType = EParamaterSlotType::SampledTextured;
}

////////////////////////////////////////////////////////////////////////////////

void GfTextureView::Init(const GfTexturedResource* pTexture, ETextureViewType::Type eViewType)
{
	if (!IsInitialised())
	{
		m_eViewType = eViewType;
		m_pTexture = pTexture;
		MarkAsInitialised();
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfTextureView::SetLodRange(u32 uiBaseLod, u32 uiLodCount)
{
	if (!IsGPUReady()) 
	{
		m_uiBaseLod = uiBaseLod;
		m_uiLodCount = uiLodCount;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfTextureView::SetArrayRange(u32 uiBaseLayer, u32 uiLayerCount)
{
	if (!IsGPUReady()) 
	{
		m_uiBaseLayerIdx = uiBaseLayer;
		m_uiLayerCount = uiLayerCount;
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfTextureView::Create(const GfRenderContext& kCtx)
{
	if (!IsGPUReady()) 
	{
		if (m_kPlatform.CreateRHI(kCtx)) 
		{
			MarkAsGPUReady();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfTextureView::Destroy(const GfRenderContext& kCtx)
{
	if (IsGPUReady()) 
	{
		m_kPlatform.DestroyRHI(kCtx);
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF