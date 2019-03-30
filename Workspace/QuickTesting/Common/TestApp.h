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

#include "GfCore/Common/GfCore.h"

#include "GfMemory/Common/GfRAII.h"
#include "GfRender/Common/GfMaterial.h"
#include "GfCore/Common/GfSingleton.h"
#include "GfRender/Common/GfRender.h"
#include "GfFile/Common/GfFile.h"

////////////////////////////////////////////////////////////////////////////////

class TestApp 
{
public:

	s32 Run(const GfEntryArgs& kEntryParams);

private:

	void Init();

	void Render(GfCmdBuffer& kCmdBuffer);

	void CreateCmdBuffers();

	void CreateRenderPasses();

	void CreateMaterialsAndParamSets();

	void CreateResources();

	void BindResourcesToParamSets();

	GfWindow		m_kWindow;
	GfRenderContext m_kContext;

	GfCmdBufferFactory	m_kCmdBufferFactory;
	GfCmdBuffer			m_pCmdBuffes[GfRenderConstants::ms_uiNBufferingCount];

	GfRenderPass		m_kRenderPass;

	GfMaterialTemplate	m_kMaterialT;
	GfMatUniformFactory	m_kUniformFactory;
	GfMatParamLayout	m_kParamLayout;
	GfMaterialParamSet	m_kParamSet;

	GfTexture2D			m_kTesTexture;

	GfViewport		m_kViewport;
	GfScissor		m_kScissor;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __TESTAPP_H__