////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/18
//	File: 	GfTexture2D.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFTEXTURE2D_H__
#define __GFTEXTURE2D_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfRender/Common/GraphicResources/GfGraphicResourceBase.h"
#include GF_SOLVE_PLATFORM_HEADER_EXT(GfRender, GraphicResources/GfTexture2D)

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

////////////////////////////////////////////////////////////////////////////////

class GfTexture2D : public GfTexturedResource
{
	GF_DECLARE_PLATFORM_INTERFACE(GfTexture2D);
public:

	GfTexture2D();

	bool Init(u32 uiWidth, u32 uiHeight, u32 uiMips,
		ETextureFormat::Type eFormat,
		const ETextureUsageBits::GfMask& uiUsage,
		const GfTexturedResource::GfFlagsMask& uiFlags);

	void Create(const GfRenderContext& kCtx);

	void Destroy(const GfRenderContext& kCtx);

	u32 GetWidth() const;

	u32 GetHeight() const;

	////////////////////////////////////////////////////////////////////////////////
	// Commands

	void LoadTexture2DDataFromStaging(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiBufferOffset);

	////////////////////////////////////////////////////////////////////////////////

private:

	u32							m_uiWidth;
	u32							m_uiheight;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfTexture2D::GetWidth() const
{
	return m_uiWidth;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfTexture2D::GetHeight() const
{
	return m_uiheight;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfTexture2D::LoadTexture2DDataFromStaging(const GfRenderContext& kCtx, const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiBufferOffset)
{
	m_kPlatform.LoadTexture2DDataFromStagingBufferRHI(kCtx, kCmdBuffer, kFrom, uiBufferOffset);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTURE2D_H__