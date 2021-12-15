////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/03/30
//	File: 	TestApp.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __TESTAPP_H__
#define __TESTAPP_H__
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfCore.h"

#include "Common/GfMemory/GfRAII.h"
#include "Common/GfCore/GfSingleton.h"
#include "Common/GfRender/GfRender.h"
#include "Common/GfFile/GfFile.h"

////////////////////////////////////////////////////////////////////////////////

class TestApp 
{
public:

	s32 Run(const GfEntryArgs& kEntryParams);

private:

	void init(const GfEntryArgs& kEntryParams);

	void Update();

	void Render(GfCmdBuffer& kCmdBuffer);

	void Shutdown();

	GfWindow		m_window;
	GfRenderContext m_context;

	GfShaderCacheFile m_shaderCache;

	GfRenderPass* m_renderPass;
	GfShaderPipeline* m_shader;

	// Camera
	GfCamera		m_kCamera;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __TESTAPP_H__