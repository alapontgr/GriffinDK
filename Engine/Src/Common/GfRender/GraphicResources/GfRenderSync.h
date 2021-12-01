#pragma once
//////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfRenderSync.h
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFRENDERSYNC_H__
#define __GFRENDERSYNC_H__
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfRender/GfRenderCommon.h"
#include GF_SOLVE_GFX_API_PATH(GfRender/GraphicResources/GfRenderSync_Platform.h)

class GfRenderContext;
class GfTexture;
class GfBuffer;

// Barriers

struct GfTextureBarrier 
{
	const GfTexture* m_texture = nullptr;
	GfTextureViewConfig m_viewConfig;
	TextureUsageFlags::Mask m_oldUsage;
	TextureUsageFlags::Mask m_newUsage;
};
struct GfTextureBarrierArray 
{
	GfTextureBarrier* m_array;
	u32 m_size = 0;
	u32 m_capacity = 0;
};
struct GfBufferBarrier 
{
	const GfBuffer* m_buffer = nullptr;
	u32 m_offset = 0;
	u32 m_size;
	BufferUsageFlags::Mask m_oldUsage;
	BufferUsageFlags::Mask m_newUsage;
};
struct GfBufferBarrierArray 
{
	GfBufferBarrier* m_array;
	u32 m_size = 0;
	u32 m_capacity = 0;
};

// Semaphores

class GfSemaphore
{
	GF_DECLARE_PLATFORM_INTERFACE(GfSemaphore);
public:

	GfSemaphore();

	void create(const GfRenderContext& ctx);

	void destroy(const GfRenderContext& ctx);
};



#endif