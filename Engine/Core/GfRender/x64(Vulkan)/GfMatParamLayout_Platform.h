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
public: 

	friend class GfMatUniformFactory;

	GfMatUniformFactory_Platform(GfMatUniformFactory& kBase);

protected:

	void CreateRHI(const GfRenderContext& kCtxt);

	void DestroyRHI(const GfRenderContext& kCtxt);

private:

	GfMatUniformFactory&	m_kBase;
	VkDescriptorPool		m_pPool;
};

////////////////////////////////////////////////////////////////////////////////

class GfMatParamLayout_Platform 
{
public:

	friend class GfMatParamLayout;

	GfMatParamLayout_Platform(GfMatParamLayout& kBase);

	VkDescriptorSetLayout GetLayout() const;

private:

	GfMatParamLayout&	m_kBase;
	VkDescriptorSetLayout	m_pSetLayout;
};

////////////////////////////////////////////////////////////////////////////////

class GfMaterialParamGroup_Platform
{
public:

	friend class GfMaterialParamGroup;

	GfMaterialParamGroup_Platform(GfMaterialParamGroup& kBase);

protected:

	void CreateRHI(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory);

	void DestroyRHI(const GfRenderContext& kCtxt, GfMatUniformFactory& kFactory);

private:

	GfMaterialParamGroup& m_kBase;
	VkDescriptorSet m_pParamatersGroup;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkDescriptorSetLayout GfMatParamLayout_Platform::GetLayout() const
{
	return m_pSetLayout;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIALPARAMLAYOUT_PLATFORM_H__