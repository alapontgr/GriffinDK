////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/07
//	File: 	GfMaterialParameterBlock_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMATERIALPARAMETERBLOCK_PLATFORM_H__
#define __GFMATERIALPARAMETERBLOCK_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfRender/Common/GfGraphicsSDK.h"

////////////////////////////////////////////////////////////////////////////////

class GfMaterialParameterBlock_Platform 
{
public:

	friend class GfMaterialParameterBlock;

	GfMaterialParameterBlock_Platform(GfMaterialParameterBlock& kBase);

private:

	GfMaterialParameterBlock&	m_kBase;
	VkDescriptorSet				m_pParameterSet;
	VkDescriptorSetLayout		m_pSetLayout[EMaterialParamRate::Count];
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMATERIALPARAMETERBLOCK_PLATFORM_H__