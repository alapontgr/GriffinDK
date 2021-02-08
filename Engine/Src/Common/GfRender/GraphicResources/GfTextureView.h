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

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfRender/GraphicResources/GfTexture.h"

#include GF_SOLVE_GFX_API_PATH(GfRender/GraphicResources/GfTextureView_Platform.h)

////////////////////////////////////////////////////////////////////////////////

struct GfExternTexView : public GfExternTexView_Platform
{
	ETextureViewType::Type m_eViewType;
	u32 m_uiBaseLod;
	u32 m_uiLodCount;
	u32 m_uiBaseLayerIdx;
	u32 m_uiLayerCount;
};

////////////////////////////////////////////////////////////////////////////////

class GfTextureView : public GfGraphicsResourceBase
{
	GF_DECLARE_PLATFORM_INTERFACE(GfTextureView);
public:

	GfTextureView();

	void init(const GfTexture* pTexture, ETextureViewType::Type eViewType); // Required

	// Init the Texture view with externally created resources (i.e. backbuffer)
	void ExternalInit(const GfExternTexView& kExternInitParams);

	void SetLodRange(u32 uiBaseLod, u32 uiLodCount);

	void SetArrayRange(u32 uiBaseLayer, u32 uilayerCount);

	////////////////////////////////////////////////////////////////////////////////

	void Create(const GfRenderContext& kCtx);

	void Destroy(const GfRenderContext& kCtx);

	////////////////////////////////////////////////////////////////////////////////

private:

	const GfTexture* m_pTexture;
	ETextureViewType::Type m_eViewType;
	u32 m_uiBaseLod;
	u32 m_uiLodCount;
	u32 m_uiBaseLayerIdx;
	u32 m_uiLayerCount;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTUREVIEW_H__