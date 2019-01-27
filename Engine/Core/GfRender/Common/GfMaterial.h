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

	void SetTopology(EPrimitiveTopology::Type val) { m_eTopology = val; }

	void SetRasterState(const GfRasterState& kVal) { m_kRasterState = kVal; }

	void SetMSState(const GfMultiSamplingState& kVal) { m_kMSState = kVal; }

	void SetBlendState(const GfBlendState& kVal) { m_kBlendState = kVal; }
	
	void SetVertexFormat(const GfVertexDeclaration& kVal) { m_kVertexFormat = kVal; }

private:

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
#endif // __GFMATERIAL_H__