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

class GfGraphicsResource;

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

	void SetMaxAllocationsPerParamType(EParamaterSlotType::Type eType, u32 uiMaxAllocations);

	void SetMaxAllocatedParamSets(u32 uiMaxSets);

	void Create(const GfRenderContext& kCtxt);

	void Destroy(const GfRenderContext& kCtxt);

private:

	u32 GetUsedTypeCount() const;

	u32 m_uiMaxAllocatedSets;
	u32 m_pCountPerUniformType[EParamaterSlotType::Count];
};

////////////////////////////////////////////////////////////////////////////////

class GfMatParamLayout : public GfMatParamLayout_Platform
{
public:

	friend class GfMatParamLayout_Platform;

	GfMatParamLayout();

	void DefineParameter(EParamaterSlotType::Type eType, GfShaderAccessMask mAccessMask, u32 uiBindSlot);

	bool Validate() const;

	u32 GetParameterCount() const; 
	
	GfMaterialParameterSlot GetAttrib(u32 uiSlot) const;

protected:

	GfVector<GfMaterialParameterSlot> m_tParameters;
};

////////////////////////////////////////////////////////////////////////////////

class GfMaterialParamSet : public GfMaterialParamSet_Platform
{
public: 

	friend class GfMaterialParamSet_Platform;

	GfMaterialParamSet();

	bool Create(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory);

	void Destroy(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory);

	bool Update(const GfRenderContext& kCtxt);

	void BindLayout(const GfMatParamLayout* pParamLayout);

	void BindResource(u32 uiSlot, const GfGraphicsResource* pResource);

private:

	enum EFlags : u32 
	{
		LayoutAssigned			= 1<<0,
		GPUResourceInitialised	= 1<<1,
		GPUDirty				= 1<<2,
		GPUUpdatePending		= 1<<3,
	};

	static constexpr u32 ms_uiMaxResources = 32;


	const GfMatParamLayout*				m_pSetLayout;
	GfVector<const GfGraphicsResource*> m_tBoundParamaters;
	GfBitMask<u32>						m_uiFlags;
	GfBitMask<u32>						m_uiDirtyResources;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATPARAMLAYOUT_H__