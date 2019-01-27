////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/26
//	File: 	GfMaterial.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMATERIAL_H__
#define __GFMATERIAL_H__
////////////////////////////////////////////////////////////////////////////////

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfRender/Common/GfMaterialShared.h"

#include GF_SOLVE_PLATFORM_HEADER(GfMaterial)

////////////////////////////////////////////////////////////////////////////////

class GfMaterialParamSet;
class GfMatParamLayout;

////////////////////////////////////////////////////////////////////////////////

class GfMaterialTemplate : public GfMaterialTemplate_Platform
{
public:

	friend class GfMaterialTemplate_Platform;

	GfMaterialTemplate();

	void Create(const GfRenderContext& kCtx);

	void Destroy(const GfRenderContext& kCtx);

	void AssignLayout(u32 uiSlot, GfMatParamLayout* pLayout);

	EPrimitiveTopology::Type GetTopology() const { return m_eTopology; }

	GfRasterState GetRasterState() const { return m_kRasterState; }

	GfMultiSamplingState GetMSState() const { return m_kMSState; }

	GfBlendState GetBlendState() const { return m_kBlendState; }

	GfVertexDeclaration GetVertexFormat() const { return m_kVertexFormat; }

	void SetTopology(EPrimitiveTopology::Type val);

	void SetRasterState(const GfRasterState& kVal);

	void SetMSState(const GfMultiSamplingState& kVal);

	void SetBlendState(const GfBlendState& kVal);
	
	void SetVertexFormat(const GfVertexDeclaration& kVal);

private:

	enum EFlags : u32
	{
		Topology_Initialised		= 1 << 0,
		RasterState_Initialised		= 1 << 1,
		MSState_Initialised			= 1 << 2,
		BlendState_Initialised		= 1 << 3,
		VertexFormat_Initialised	= 1 << 4,
	};

	u32 GetBoundLayoutCount() const;

	// Primitive topology
	EPrimitiveTopology::Type	m_eTopology;
	// Raster state
	GfRasterState				m_kRasterState;
	// MultiSampling state
	GfMultiSamplingState		m_kMSState;
	// Blend state
	GfBlendState				m_kBlendState;
	// Vertex declaration
	GfVertexDeclaration			m_kVertexFormat;
	// Description of parameter layouts
	GfMatParamLayout*			m_pLayouts[EMaterialParamRate::MaxBoundSets];
	// Flags
	GfBitMask<u32>				m_uiFlags;
};

////////////////////////////////////////////////////////////////////////////////

class GfMaterialinstance
{
public:

	GfMaterialinstance();

	void BindParameterSet(GfMaterialParamSet* pSet);

private:
	
	GfMaterialTemplate* m_pTemplate;
	GfMaterialParamSet* m_pBoundSets[EMaterialParamRate::MaxBoundSets];
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfMaterialTemplate::SetTopology(EPrimitiveTopology::Type val)
{
	m_uiFlags |= EFlags::Topology_Initialised;
	m_eTopology = val;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfMaterialTemplate::SetRasterState(const GfRasterState& kVal)
{
	m_uiFlags |= EFlags::RasterState_Initialised;
	m_kRasterState = kVal;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfMaterialTemplate::SetMSState(const GfMultiSamplingState& kVal)
{
	m_uiFlags |= EFlags::MSState_Initialised;
	m_kMSState = kVal;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfMaterialTemplate::SetBlendState(const GfBlendState& kVal)
{
	m_uiFlags |= EFlags::BlendState_Initialised;
	m_kBlendState = kVal;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfMaterialTemplate::SetVertexFormat(const GfVertexDeclaration& kVal)
{
	m_uiFlags |= EFlags::VertexFormat_Initialised;
	m_kVertexFormat = kVal;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIAL_H__