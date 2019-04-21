////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/06
//	File: 	GfGraphicResourceBase.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFGRAPHICRESOURCEBASE_H__
#define __GFGRAPHICRESOURCEBASE_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfRender/Common/GfMaterialShared.h"
#include "GfRender/Common/GraphicResources/GfGraphicResourcesShared.h"
#include "GfRender/Common/GraphicResources/GfBuffer.h"

#include GF_SOLVE_PLATFORM_HEADER_EXT(GfRender, GraphicResources/GfGraphicResourceBase)

////////////////////////////////////////////////////////////////////////////////

class GfGraphicsResourceBase 
{
public:

	GfGraphicsResourceBase();

	bool IsBufferedResource() const;

	bool IsTexturedResource() const;

	EParamaterSlotType::Type GetResourceType() const;

	bool IsGPUReady() const;

	bool IsInitialised() const;

protected:

	enum EGraphicResFlags : u32 
	{
		GPUReady			= 1<<0,
		Initialised			= 1<<1,
		BufferedResource	= 1<<2,
		TexturedResource	= 1<<3,
		////////////////////////////////////////////////////////////////////////////////
		End					= 4,
	};

	void MarkAsDestroyed();

	void MarkAsGPUReady();

	void MarkAsInitialised();

	EParamaterSlotType::Type m_eResourceType;
	GfBitMask<u32>	m_uiGraphicResFlags;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE EParamaterSlotType::Type GfGraphicsResourceBase::GetResourceType() const
{
	return m_eResourceType;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfGraphicsResourceBase::IsBufferedResource() const
{
	return m_uiGraphicResFlags.IsEnable(EGraphicResFlags::BufferedResource);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfGraphicsResourceBase::IsTexturedResource() const
{
	return m_uiGraphicResFlags.IsEnable(EGraphicResFlags::TexturedResource);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfGraphicsResourceBase::IsGPUReady() const 
{
	return m_uiGraphicResFlags.IsEnable(EGraphicResFlags::GPUReady);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfGraphicsResourceBase::IsInitialised() const
{
	return m_uiGraphicResFlags.IsEnable(EGraphicResFlags::Initialised);
}

////////////////////////////////////////////////////////////////////////////////

class GfBufferedResource : public GfGraphicsResourceBase
{
public:

	GfBufferedResource();

	void BindBuffer(const GfBuffer::GfRange& kRange);

	u32 GetSize() const { return m_kBufferRange.m_uiSize; }

	const GfBuffer::GfRange GetBufferRange() const { return m_kBufferRange; }

protected:

	enum EBUfferedResFlags : u32 
	{
		BufferBound = 1<<GfGraphicsResourceBase::EGraphicResFlags::End,
	};

	GfBuffer::GfRange m_kBufferRange;
};

////////////////////////////////////////////////////////////////////////////////

class GfTexturedResource : public GfGraphicsResourceBase
{
public:

	enum ETexture2DFlags : u16
	{
		// User flags
		Mappable = 1 << 0,
		Tilable = 1 << 1,
	};
	using GfFlagsMask = GfBitMask<u16>;

	GfTexturedResource();

	GfTexturedResource_Platform& GetSharedPlatform();
	const GfTexturedResource_Platform& GetSharedPlatformC() const;

	u32 GetMipMapCount() const;

	ETextureFormat::Type GetFormat() const;

	bool IsUsageValid(ETextureUsageBits::Type eUsage) const;

	bool IsMappable() const;

	bool IsDepthBuffer() const;

	bool IsStencilBuffer() const;

	bool IsTilable() const;

protected:

	enum EPrivateFlags : u16
	{
		// Continue from public flags
		DepthBuffer = (1 << 2),
		StencilBuffer = (1 << 3),
	};

	void SetUsage(const ETextureUsageBits::GfMask& uiUsage);
	void SetFormat(ETextureFormat::Type eFormat);
	void SetMips(u32 uiMipsCount);
	void SetTextureFlags(const GfFlagsMask& uiFlags);

	GfTexturedResource_Platform m_kCommonPlatform;
	ETextureUsageBits::GfMask	m_uiUsage;
	u32							m_uiMips;
	ETextureFormat::Type		m_eFormat;
	GfFlagsMask					m_uiTextureFlags;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE GfTexturedResource_Platform& GfTexturedResource::GetSharedPlatform()
{
	return m_kCommonPlatform;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE const GfTexturedResource_Platform& GfTexturedResource::GetSharedPlatformC() const
{
	return m_kCommonPlatform;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE u32 GfTexturedResource::GetMipMapCount() const
{
	return m_uiMips;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE ETextureFormat::Type GfTexturedResource::GetFormat() const
{
	return m_eFormat;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexturedResource::IsUsageValid(ETextureUsageBits::Type eUsage) const
{
	return m_uiUsage.IsEnable(eUsage);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexturedResource::IsMappable() const
{
	return m_uiTextureFlags.IsEnable(ETexture2DFlags::Mappable);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexturedResource::IsDepthBuffer() const
{
	return m_uiTextureFlags.IsEnable(EPrivateFlags::DepthBuffer);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexturedResource::IsStencilBuffer() const
{
	return m_uiTextureFlags.IsEnable(EPrivateFlags::StencilBuffer);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfTexturedResource::IsTilable() const
{
	return m_uiTextureFlags.IsEnable(ETexture2DFlags::Tilable);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFGRAPHICRESOURCEBASE_H__