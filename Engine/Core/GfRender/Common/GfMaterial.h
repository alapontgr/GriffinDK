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

private:

	GfMatParamLayout* m_pLayouts[EMaterialParamRate::MaxBoundSets];

	// Primitive topology
	EPrimitiveTopology::Type m_eTopology;
	
	// Raster state
	GfRasterState m_kRasterState;
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