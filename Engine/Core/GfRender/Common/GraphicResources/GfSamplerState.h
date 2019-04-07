////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/04
//	File: 	GfSamplerState.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFSAMPLERSTATE_H__
#define __GFSAMPLERSTATE_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfRender/Common/GraphicResources/GfGraphicResourceBase.h"
#include GF_SOLVE_PLATFORM_HEADER_EXT(GfRender, GraphicResources/GfSamplerState)

////////////////////////////////////////////////////////////////////////////////

class GfSamplerState : public GfGraphicsResourceBase
{
	GF_DECLARE_PLATFORM_INTERFACE(GfSamplerState);
public:

	GfSamplerState();

	void Init();

	void Create(const GfRenderContext& kCtxt);

	void Destroy(const GfRenderContext& kCtxt);

	ETexFilter::Type GetMinFilter() const { return m_eMinFilter; }
	
	ETexFilter::Type GetMagFilter() const { return m_eMagFilter; }
	
	ESamplerMipMapMode::Type GetMipMapMode() const { return m_eMipMapMode; }
	
	ETexAddressMode::Type GetAddrU() const { return m_eAddrU; }
	
	ETexAddressMode::Type GetAddrV() const { return m_eAddrV; }
	
	ETexAddressMode::Type GetAddrW() const { return m_eAddrW; }
	
	f32 GetMipLodBias() const { return m_fMipLodBias; }
	
	f32 GetMinLod() const { return m_fMinLod; }
	
	f32 GetMaxLod() const { return m_fMaxLod; }
	
	f32 GetMaxAnisotropy() const { return m_fMaxAnisotropy; }
	
	bool GetUseAnisotropy() const { return m_bUseAnisotropy; }
	
	void SetMinFilter(ETexFilter::Type eFilter);
	
	void SetMagFilter(ETexFilter::Type eFilter);
	
	void SetMipMapMode(ESamplerMipMapMode::Type eMipMapMode);
	
	void SetAddrU(ETexAddressMode::Type eAddrMode);
	
	void SetAddrV(ETexAddressMode::Type eAddrMode);
	
	void SetAddrW(ETexAddressMode::Type eAddrMode);
	
	void SetMipLodBias(f32 fBias);
	
	void SetMinLod(f32 fMinLod);
	
	void SetMaxLod(f32 fMaxLod);
	
	void SetMaxAnisotropy(f32 fMaxAnisotropy);
	
	void SetUseAnisotropy(bool bUseAnisotropy);

private:

	// Filtering & Addressing
	ETexFilter::Type			m_eMinFilter;		// Default: ETexFilter::Linear
	ETexFilter::Type			m_eMagFilter;		// Default: ETexFilter::Linear
	ESamplerMipMapMode::Type	m_eMipMapMode;		// Default: ESamplerMipMapMode::Linear
	ETexAddressMode::Type		m_eAddrU;			// Default: ETexAddressMode::ClampToEdge
	ETexAddressMode::Type		m_eAddrV;			// Default: ETexAddressMode::ClampToEdge 
	ETexAddressMode::Type		m_eAddrW;			// Default: ETexAddressMode::ClampToEdge
	f32							m_fMipLodBias;		// Default: 0.0f
	// Lod range
	f32							m_fMinLod;			// Default: 0.0f
	f32							m_fMaxLod;			// Default: 1.0f
	// Anisotropy
	f32							m_fMaxAnisotropy;	// Default: 1.0f
	bool						m_bUseAnisotropy;	// Default: false
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFSAMPLERSTATE_H__