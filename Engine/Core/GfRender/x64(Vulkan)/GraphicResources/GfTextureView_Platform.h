////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/21
//	File: 	GfTextureView_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFTEXTUREVIEW_PLATFORM_H__
#define __GFTEXTUREVIEW_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

////////////////////////////////////////////////////////////////////////////////

class GfTextureView_Platform 
{
	GF_DECLARE_PLATFORM_MEMBERS(GfTextureView);
public:

	bool CreateRHI(const GfRenderContext& kCtx);

	void DestroyRHI(const GfRenderContext& kCtx);

	VkImageView GetView() const;

private:

	VkImageView	m_pImageView;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkImageView GfTextureView_Platform::GetView() const
{
	return m_pImageView;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFTEXTUREVIEW_PLATFORM_H__