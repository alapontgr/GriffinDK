////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/04
//	File: 	GfSampler.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFSAMPLERSTATE_H__
#define __GFSAMPLERSTATE_H__
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfRender/GraphicResources/GfGraphicResourcesShared.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GraphicResources/GfSampler_Platform.h)

////////////////////////////////////////////////////////////////////////////////

class GfTextureView;

////////////////////////////////////////////////////////////////////////////////

struct SamplerDesc 
{
	// Filtering & Addressing
	ImageFilter::Type			m_minFilter;		// Default: ETexFilter::Linear
	ImageFilter::Type			m_magFilter;		// Default: ETexFilter::Linear
	SamplerMipMapMode::Type		m_mipMapMode;		// Default: ESamplerMipMapMode::Linear
	ImageAddressMode::Type		m_addrU;			// Default: ETexAddressMode::ClampToEdge
	ImageAddressMode::Type		m_addrV;			// Default: ETexAddressMode::ClampToEdge 
	ImageAddressMode::Type		m_addrW;			// Default: ETexAddressMode::ClampToEdge
	f32							m_mipLodBias;		// Default: 0.0f
	// Lod range
	f32							m_minLod;			// Default: 0.0f
	f32							m_maxLod;			// Default: 1.0f
	// Anisotropy
	f32							m_maxAnisotropy;	// Default: 1.0f
	bool						m_useAnisotropy;	// Default: false
};

class GfSampler
{
	GF_DECLARE_PLATFORM_INTERFACE(GfSampler);
public:

	friend class ResourceFactory<GfSampler>;
	friend class ResourceFactory<GfSampler>::Pool;

	enum Flags : u32 
	{
		Initialized = 1<<0,
	};

	static GfSampler* newSampler();

	bool create(const GfRenderContext& ctx, const SamplerDesc& desc);

	void release();

	void init();

	ImageFilter::Type GetMinFilter() const { return m_desc.m_minFilter; }
	
	ImageFilter::Type GetMagFilter() const { return m_desc.m_magFilter; }
	
	SamplerMipMapMode::Type GetMipMapMode() const { return m_desc.m_mipMapMode; }
	
	ImageAddressMode::Type GetAddrU() const { return m_desc.m_addrU; }
	
	ImageAddressMode::Type GetAddrV() const { return m_desc.m_addrV; }
	
	ImageAddressMode::Type GetAddrW() const { return m_desc.m_addrW; }
	
	f32 GetMipLodBias() const { return m_desc.m_mipLodBias; }
	
	f32 GetMinLod() const { return m_desc.m_minLod; }
	
	f32 GetMaxLod() const { return m_desc.m_maxLod; }
	
	f32 GetMaxAnisotropy() const { return m_desc.m_maxAnisotropy; }
	
	bool GetUseAnisotropy() const { return m_desc.m_useAnisotropy; }
	
	bool getIsInitialized() const { return (m_flags & Flags::Initialized) != 0; }

private:

	GfSampler();

	void destroy(const GfRenderContext& ctx);

	SamplerDesc m_desc;
	u32 m_flags;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFSAMPLERSTATE_H__