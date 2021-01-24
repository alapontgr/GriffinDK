////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/26
//	File: 	GfMaterial_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMATERIAL_PLATFORM_H__
#define __GFMATERIAL_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GfGraphicsSDK.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;
class GfCmdBuffer;

////////////////////////////////////////////////////////////////////////////////

class GfMaterialTemplate_Platform 
{
	GF_DECLARE_PLATFORM_MEMBERS(GfMaterialTemplate);
public:
	
	VkPipelineLayout GetLayout() const { return m_pLayout; }

	VkPipeline GetPipeline() const { return m_pPipeline; }

protected:

	struct GfShaderDesc_Platform
	{
		VkShaderModule m_pModule = VK_NULL_HANDLE;
	};

	void DestroyRHI(const GfRenderContext& kCtx);

	bool CreateRHI(const GfRenderContext& kCtx);

	////////////////////////////////////////////////////////////////////////////////

	void BindRHI(const GfCmdBuffer& kCmdBuffer);

	void PushConstantsRHI(const GfCmdBuffer& kCmdBuffer, u32 uiSize, void* pData);

	////////////////////////////////////////////////////////////////////////////////

private:

	bool CreateLayout(const GfRenderContext& kCtx);

	bool CreatePipeline(const GfRenderContext& kCtx);

	VkPipelineLayout		m_pLayout;
	VkPipeline				m_pPipeline;

	VkShaderStageFlags		m_uiPushConstantsStage;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIAL_PLATFORM_H__