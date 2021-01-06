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

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfRender/GfRenderCommon.h"
#include "Common/GfRender/GfMaterialShared.h"

#include GF_SOLVE_GFX_API_PATH(GfRender/GfMaterial_Platform.h)

////////////////////////////////////////////////////////////////////////////////

class GfMaterialParamSet;
class GfMatParamLayout;
class GfRenderPass;

////////////////////////////////////////////////////////////////////////////////

class GfMaterialTemplate
{
	GF_DECLARE_PLATFORM_INTERFACE(GfMaterialTemplate);
public:

	GfMaterialTemplate();

	void Create(const GfRenderContext& kCtx);

	void Destroy(const GfRenderContext& kCtx);

	void AssignLayout(u32 uiSlot, const GfMatParamLayout* pLayout);

	EPrimitiveTopology::Type GetTopology() const { return m_eTopology; }

	GfRasterState GetRasterState() const { return m_kRasterState; }

	GfMultiSamplingState GetMSState() const { return m_kMSState; }

	GfBlendState GetBlendState() const { return m_kBlendState; }

	GfVertexDeclaration GetVertexFormat() const { return m_kVertexFormat; }

	const GfRenderPass* GetMaterialPass() const { return m_pMaterialPass; }

	bool IsFullyInitialised() const { return (m_uiFlags&EFlags::FullyInitialised) == EFlags::FullyInitialised; }

	void SetTopology(EPrimitiveTopology::Type val);

	void SetRasterState(const GfRasterState& kVal);

	void SetMSState(const GfMultiSamplingState& kVal);

	void SetBlendState(const GfBlendState& kVal);
	
	void SetVertexFormat(const GfVertexDeclaration& kVal);
	
	void SetMaterialPass(const GfRenderPass* val);

	void SetShaderData(EShaderStage::Type eStage, const char* szEntry, const char* pSrc, u32 uiSrcDataSize);

	void DeclareConstantsBlock(const GfShaderAccessMask& uiStages, u32 uiSize);

	////////////////////////////////////////////////////////////////////////////////
	// Commands

	void Bind(const GfCmdBuffer& kCmdBuffer);

	// pData is expected to have at least "m_uiConstantsBlockSize" size
	void PushConstants(const GfCmdBuffer& kCmdBuffer, void* pData);

	////////////////////////////////////////////////////////////////////////////////

private:

	enum EFlags : u32
	{
		Topology_Initialised		= 1 << 0,
		RasterState_Initialised		= 1 << 1,
		MSState_Initialised			= 1 << 2,
		BlendState_Initialised		= 1 << 3,
		VertexFormat_Initialised	= 1 << 4,
		MaterialPass_Initialised	= 1 << 5,
		Constants_Initialised		= 1 << 6,
		GPUresource_Initialised		= 1 << 7,
		FullyInitialised			= ((1<<8)-1)
	};

	struct GfShaderDesc : public GfMaterialTemplate_Platform::GfShaderDesc_Platform
	{
		const char* m_pSourceData = nullptr;
		const char* m_szEntryPoint = nullptr;
		u32			m_uiSrcDataSize = 0;
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
	
	// Material pass
	const GfRenderPass*			m_pMaterialPass;
	
	// Description of parameter layouts
	const GfMatParamLayout*			m_pLayouts[EMaterialParamRate::MaxBoundSets];
	
	// Stages
	GfShaderDesc				m_pShaderStages[EShaderStage::COUNT];

	// Constants
	GfShaderAccessMask			m_uiConstantsStages;
	u32							m_uiConstantsBlockSize;

	// Flags
	GfBitMask<u32>				m_uiFlags;
};

////////////////////////////////////////////////////////////////////////////////

class GfMaterialinstance
{
public:

	GfMaterialinstance();

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

GF_FORCEINLINE void GfMaterialTemplate::SetMaterialPass(const GfRenderPass* val)
{
	m_uiFlags |= EFlags::MaterialPass_Initialised;
	m_pMaterialPass = val;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfMaterialTemplate::DeclareConstantsBlock(const GfShaderAccessMask& uiStages, u32 uiSize)
{
	m_uiConstantsStages = uiStages;
	m_uiConstantsBlockSize = uiSize;
	m_uiFlags |= EFlags::Constants_Initialised;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfMaterialTemplate::Bind(const GfCmdBuffer& kCmdBuffer)
{
	m_kPlatform.BindRHI(kCmdBuffer);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfMaterialTemplate::PushConstants(const GfCmdBuffer& kCmdBuffer, void* pData)
{
	if (m_uiFlags & EFlags::Constants_Initialised) 
	{
		m_kPlatform.PushConstantsRHI(kCmdBuffer, m_uiConstantsBlockSize, pData);
	}
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIAL_H__