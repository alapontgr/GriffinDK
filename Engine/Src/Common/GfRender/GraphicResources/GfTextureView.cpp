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

#include "Common/GfRender/GraphicResources/GfTextureView.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfTextureView) 
	, m_pTexture(nullptr)
	, m_eViewType(ETextureViewType::View2D)
	, m_uiBaseLod(0)
	, m_uiLodCount(1)
	, m_uiBaseLayerIdx(0)
	, m_uiLayerCount(1)
{
	m_eResourceType = EParamaterSlotType::Texture;
}

////////////////////////////////////////////////////////////////////////////////

void GfTextureView::init(const GfTexture* pTexture, ETextureViewType::Type eViewType)
{
	if (!IsInitialised())
	{
		m_eViewType = eViewType;
		m_pTexture = pTexture;
		MarkAsInitialised();
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfTextureView::ExternalInit(const GfExternTexView& kExternInitParams)
{
	if (!IsInitialised() || IsExternallyInitialized()) 
	{
		m_uiBaseLod = kExternInitParams.m_uiBaseLod;
		m_uiLodCount = kExternInitParams.m_uiLodCount;
		m_uiBaseLayerIdx = kExternInitParams.m_uiBaseLayerIdx;
		m_uiLayerCount = kExternInitParams.m_uiLayerCount;
		m_kPlatform.ExternInitPlat(kExternInitParams);
		MarkAsInitialised();
		MarkAsGPUReady();
		MarkAsExternallyInitiailized();
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
		if (m_kPlatform.createRHI(kCtx)) 
		{
			MarkAsGPUReady();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void GfTextureView::Destroy(const GfRenderContext& kCtx)
{
	if (IsGPUReady() && !IsExternallyInitialized()) 
	{
		m_kPlatform.destroyRHI(kCtx);
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF