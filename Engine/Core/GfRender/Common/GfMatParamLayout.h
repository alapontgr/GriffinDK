////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/07
//	File: 	GfMaterialParameterBlock.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMATPARAMLAYOUT_H__
#define __GFMATPARAMLAYOUT_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfCore/Common/GfCoreMinimal.h"
#include "GfCore/Common/GfStl.h"
#include "GfRender/Common/GfMaterialShared.h"
#include GF_SOLVE_PLATFORM_HEADER(GfMatParamLayout)

////////////////////////////////////////////////////////////////////////////////

struct GfMaterialParameterSlot
{
	EParamaterSlotType::Type	m_eType;
	GfShaderAccessMask			m_AccesStages;
	u32							m_uiBindSlot;
};

////////////////////////////////////////////////////////////////////////////////

class GfMatUniformFactory : public GfMatUniformFactory_Platform
{
public:

	friend class GfMatUniformFactory_Platform;

	GfMatUniformFactory();

private:

	u32 m_pCountPerUniformType[EParamaterSlotType::Count];
};

////////////////////////////////////////////////////////////////////////////////

class GfMatParamLayout : public GfMatParamLayout_Platform
{
public:

	friend class GfMatParamLayout_Platform;

	GfMatParamLayout();

	void DefineParameter(EParamaterSlotType::Type eType, GfShaderAccessMask mAccessMask, u32 uiBindSlot);

	static u8 ms_uiInvalidValue;

private:

	void SortParameters();

	GfVector<GfMaterialParameterSlot> m_tParameters;
};

////////////////////////////////////////////////////////////////////////////////

class GfMaterialParamGroup : public GfMaterialParamGroup_Platform
{
public: 

	friend class GfMaterialParamGroup_Platform;

	GfMaterialParamGroup();

	void BindLayout(const GfMatParamLayout* pParamLayout, EMaterialParamRate::Type eRate);

	void Create(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory);

	void Destroy(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory);

private:
		
	const GfMatParamLayout* m_pPerRateLayouts[EMaterialParamRate::Count];
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATPARAMLAYOUT_H__