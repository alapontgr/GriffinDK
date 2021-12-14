////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/04
//	File: 	GfSampler.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GraphicResources/GfSampler.h"

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_BASE_CTOR(GfSampler)
	, m_desc{}
	, m_flags(0)
{
}

////////////////////////////////////////////////////////////////////////////////

GfSampler* GfSampler::newSampler()
{
	return ResourceFactory<GfSampler>::create();
}

bool GfSampler::create(const GfRenderContext& ctx, const SamplerDesc& desc)
{
	if (!getIsInitialized())
	{
		m_desc = desc;
		m_kPlatform.create(ctx);
		m_flags |= Flags::Initialized;
		return true;
	}
	return false;
}

void GfSampler::release()
{
	ResourceFactory<GfSampler>::destroy(this);
}

////////////////////////////////////////////////////////////////////////////////

void GfSampler::destroy(const GfRenderContext& ctx)
{
	m_kPlatform.destroy(ctx);
	m_flags = 0;
}

////////////////////////////////////////////////////////////////////////////////
// EOF