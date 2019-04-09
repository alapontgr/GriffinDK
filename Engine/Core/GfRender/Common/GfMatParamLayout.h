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

class GfGraphicsResourceBase;

////////////////////////////////////////////////////////////////////////////////

struct GfMaterialParameterSlot
{
	static constexpr u32 ms_uiInvalidSlotIdx = 0xffffffff;


	EParamaterSlotType::Type	m_eType;
	GfShaderAccessMask			m_AccesStages;
	u32							m_uiBindSlot;
};
using GfParamSlotIdx = u32;

////////////////////////////////////////////////////////////////////////////////

class GfMatUniformFactory
{
	GF_DECLARE_PLATFORM_INTERFACE(GfMatUniformFactory)
public:

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

class GfMatParamLayout
{
	GF_DECLARE_PLATFORM_INTERFACE(GfMatParamLayout)
public:

	GfMatParamLayout();

	void Create(const GfRenderContext& kCtxt);

	void Destroy(const GfRenderContext& kCtxt);

	void DefineParameter(EParamaterSlotType::Type eType, GfShaderAccessMask mAccessMask, u32 uiBindSlot);

	u32 GetParameterCount() const; 
	
	GfMaterialParameterSlot GetAttrib(GfParamSlotIdx uiIdx) const;

	GfParamSlotIdx GetResourceIdx(EParamaterSlotType::Type eResType, u32 uiBoundSlot);

	bool IsGPUInitialised() const;

protected:

	enum EFlags : u32 
	{
		GPU_Initialised = 1<<0,
	};

	GfVector<GfMaterialParameterSlot>	m_tParameters;
	u32									m_uiFlags;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfMatParamLayout::IsGPUInitialised() const
{
	return (m_uiFlags & EFlags::GPU_Initialised) != 0;
}

////////////////////////////////////////////////////////////////////////////////

class GfMaterialParamSet
{
	GF_DECLARE_PLATFORM_INTERFACE(GfMaterialParamSet)
public: 

	GfMaterialParamSet();

	////////////////////////////////////////////////////////////////////////////////
	// Resource creation

	// 1.
	void Init(const GfMatParamLayout* pParamLayout);

	// 2.
	bool Create(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory);

	////////////////////////////////////////////////////////////////////////////////
	// Updating the resource

	bool BindResource(GfParamSlotIdx uiSlotIdx, const GfGraphicsResourceBase* pResource);

	bool Update(const GfRenderContext& kCtxt);

	////////////////////////////////////////////////////////////////////////////////

	void Destroy(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory);

	////////////////////////////////////////////////////////////////////////////////
	// Commands

	void BindSet(const GfCmdBuffer& kCmdBuffer, const GfMaterialTemplate& kMaterial,u32 uiBindPoint = 0, bool bIsGraphics = true);

	////////////////////////////////////////////////////////////////////////////////

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
	GfVector<const GfGraphicsResourceBase*> m_tBoundParamaters;
	GfBitMask<u32>						m_uiFlags;
	GfBitMask<u32>						m_uiDirtyResources;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATPARAMLAYOUT_H__