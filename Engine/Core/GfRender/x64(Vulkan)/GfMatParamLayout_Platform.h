////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/07
//	File: 	GfMaterialParameterBlock_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMATERIALPARAMLAYOUT_PLATFORM_H__
#define __GFMATERIALPARAMLAYOUT_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfGraphicsSDK.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;

////////////////////////////////////////////////////////////////////////////////

class GfMatUniformFactory_Platform 
{
	GF_DECLARE_PLATFORM_MEMBERS(GfMatUniformFactory);
public: 

	VkDescriptorPool GetPool() const;

protected:

	void CreateRHI(const GfRenderContext& kCtxt);

	void DestroyRHI(const GfRenderContext& kCtxt);

private:

	VkDescriptorPool		m_pPool;
};

////////////////////////////////////////////////////////////////////////////////

class GfMatParamLayout_Platform 
{
	GF_DECLARE_PLATFORM_MEMBERS(GfMatParamLayout);
public:

	VkDescriptorSetLayout GetLayout() const;

	bool CreateRHI(const GfRenderContext& kCtxt);

	void DestroyRHI(const GfRenderContext& kCtxt);

private:

	VkDescriptorSetLayout	m_pSetLayout;
};

////////////////////////////////////////////////////////////////////////////////

class GfMaterialParamSet_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfMaterialParamSet);
public:

	VkDescriptorSet GetDescriptorSet() const { return m_pParamatersSet; }

protected:

	bool CreateRHI(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory);

	void DestroyRHI(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory);

	void UpdateRHI(const GfRenderContext& kCtxt);

private:

	VkDescriptorSet		m_pParamatersSet;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkDescriptorPool GfMatUniformFactory_Platform::GetPool() const
{
	return m_pPool;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkDescriptorSetLayout GfMatParamLayout_Platform::GetLayout() const
{
	return m_pSetLayout;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIALPARAMLAYOUT_PLATFORM_H__