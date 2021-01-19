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
#include "Common/GfRender/GfMaterial.h"
#include "Common/GfRender/GfRender.h"
#include "Common/GfFile/GfFile.h"

////////////////////////////////////////////////////////////////////////////////

class TestApp 
{
public:

	s32 Run(const GfEntryArgs& kEntryParams);

private:

	void Init(const GfEntryArgs& kEntryParams);

	void Update();

	void Render(GfCmdBuffer& kCmdBuffer);

	void Shutdown();

	void DoTransferOperations(GfCmdBuffer& kCmdBuffer);

	void CreateCmdBuffers();

	void CreateRenderPasses();

	void CreateMaterialsAndParamSets();

	void CreateResources();

	void loadGLTF();

	GfWindow		m_kWindow;
	GfRenderContext m_kContext;

	GfCmdBufferFactory	m_kCmdBufferFactory;
	GfCmdBuffer			m_pCmdBuffes[GfRenderConstants::ms_uiNBufferingCount];

	GfRenderPass		m_kRenderPass;

	GfBuffer			m_kStagingBuffer;
	GfBuffer			m_kConstantBufferPool;

	GfMaterialTemplate	m_kMaterialT;
	GfMatUniformFactory	m_kUniformFactory;
	GfMatParamLayout	m_kParamLayout;
	GfMaterialParamSet	m_kParamSet;

	GfConstantBuffer	m_kPerFrameCB;
	GfSamplerState		m_kSampler;
	GfCombinedSamplerTexture m_kCombinedSamplerTexture;
	GfTexture2D			m_kTesTexture;
	GfTextureView		m_kTestTexView;
	GfUniquePtr<char[]>	m_pTestTextureData;

	GfViewport		m_kViewport;
	GfScissor		m_kScissor;

	// Camera
	GfCamera		m_kCamera;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __TESTAPP_H__