////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/21
//	File: 	GfTextureView.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFTEXTUREVIEW_H__
#define __GFTEXTUREVIEW_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfRender/Common/GraphicResources/GfGraphicResourceBase.h"

#include GF_SOLVE_PLATFORM_HEADER_EXT(GfRender, GraphicResources/GfTextureView)

////////////////////////////////////////////////////////////////////////////////

class GfTextureView : public GfGraphicsResourceBase
{
	GF_DECLARE_PLATFORM_INTERFACE(GfTextureView);
public:

	GfTextureView();

	void Init(const GfTexturedResource* pTexture, ETextureViewType::Type eViewType); // Required

	void SetLodRange(u32 uiBaseLod, u32 uiLodCount);

	void SetArrayRange(u32 uiBaseLayer, u32 uilayerCount);

	////////////////////////////////////////////////////////////////////////////////

	void Create(const GfRenderContext& kCtx);

	void Destroy(const GfRenderContext& kCtx);

	////////////////////////////////////////////////////////////////////////////////

private:

	const GfTexturedResource* m_pTexture;
	ETextureViewType::Type m_eViewType;
	u32 m_uiBaseLod;
	u32 m_uiLodCount;
	u32 m_uiBaseLayerIdx;
	u32 m_uiLayerCount;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTUREVIEW_H__