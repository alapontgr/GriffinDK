////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/03/30
//	File: 	TestApp.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "QuickTesting/Common/TestApp.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

////////////////////////////////////////////////////////////////////////////////

GfUniquePtr<char[]> LoadFileSrc(const char* szFilepath, u32& uiOutFileSize)
{
	GfFileHandle kFile;
	GfFile::OpenFile(szFilepath, EFileAccessMode::Read, kFile);
	GfFile::GetFileSize(kFile);
	uiOutFileSize = static_cast<u32>(kFile.GetFileSize());
	GfUniquePtr<char[]> pSrc(new char[uiOutFileSize]);
	u32 uiRead = GfFile::ReadBytes(kFile, uiOutFileSize, pSrc.get());
	GfFile::CloseFile(kFile);
	GF_ASSERT(uiRead == uiOutFileSize, "Invalid size read");
	return pSrc;
}

////////////////////////////////////////////////////////////////////////////////

GfUniquePtr<char[]> LoadTexture(const char* szTexturePath, s32& siWidth, s32& siHeight, s32& siComponentCount) 
{
	// Force 4 components
	unsigned char *pData = stbi_load(szTexturePath, &siWidth, &siHeight, &siComponentCount, 4);
	size_t uiTextureSize((size_t)siWidth * siHeight * siComponentCount);
	
	GfUniquePtr<char[]> pSrc(new char[uiTextureSize]);
	memcpy(pSrc.get(), pData, uiTextureSize);
	stbi_image_free(pData);
	return pSrc;
}

////////////////////////////////////////////////////////////////////////////////

s32 TestApp::Run(const GfEntryArgs& kEntryParams)
{
	Init();
	
	while (m_kContext.BeginFrame())
	{
		u32 uiCurrFrameIdx(m_kContext.GetCurrentFrameIdx());
		GfCmdBuffer& kCmdBuffer(m_pCmdBuffes[uiCurrFrameIdx]);

		// Wait for the command buffer to be ready
		kCmdBuffer.WaitForReady(m_kContext);

		// Begin the Command buffer
		kCmdBuffer.BeginRecording(m_kContext);

		Render(kCmdBuffer);

		// End the command buffer
		kCmdBuffer.EndRecording(m_kContext);

		// Submit command buffer
		kCmdBuffer.Submit(m_kContext, GfRenderContextFamilies::Present, GF_TRUE);

		m_kContext.EndFrame();
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::Init()
{
	// Create the window
	GfWindowInitParams kWindowInit;
	kWindowInit.m_uiWidth = 512;
	kWindowInit.m_uiHeight = 512;
	kWindowInit.m_bVSync = false;
	kWindowInit.m_bFullScreen = false;
	kWindowInit.m_szWindowName = "TestGriffinApp";
	m_kWindow.Init(kWindowInit);

	// Init the render context
	m_kContext.Init(&m_kWindow);
	////////////////////////////////////////////////////////////////////////////////

	// Create resources
	CreateRenderPasses();
	CreateMaterialsAndParamSets();
	CreateCmdBuffers();
	CreateResources();
	BindResourcesToParamSets();
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::CreateCmdBuffers()
{
	m_kCmdBufferFactory.Init(m_kContext, GfRenderContextFamilies::Graphics);
	for (u32 i = 0; i < GfRenderConstants::ms_uiNBufferingCount; ++i)
	{
		m_kCmdBufferFactory.CreateCmdBuffer(m_kContext, GfCmdBufferType::Primary, m_pCmdBuffes[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::CreateRenderPasses()
{
	m_kRenderPass.Create(m_kContext);

	// Init Viewport and Scissor state
	m_kViewport.m_fWidth = (f32)m_kWindow.GetWidth();
	m_kViewport.m_fHeight = (f32)m_kWindow.GetHeight();

	m_kScissor.m_siWidth = (s32)m_kViewport.m_fWidth;
	m_kScissor.m_siHeight = (s32)m_kViewport.m_fHeight;
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::Render(GfCmdBuffer& kCmdBuffer)
{
	// Begin render pass
	m_kRenderPass.BeginPass(m_kContext, kCmdBuffer);

	// Set Viewport and Scissor
	m_kRenderPass.SetViewport(kCmdBuffer, m_kViewport);
	m_kRenderPass.SetScissor(kCmdBuffer, m_kScissor);

	// Add commands
	m_kMaterialT.Bind(kCmdBuffer);

	// Draw
	kCmdBuffer.Draw(6, 1);

	// End render pass
	m_kRenderPass.EndPass(kCmdBuffer);
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::CreateMaterialsAndParamSets()
{
	// Create material
	u32 uiVertexSize, uiFragmentSize;
	GfUniquePtr<char[]> pVertexSrc(LoadFileSrc("Shaders/bin/dummy.vert.spv", uiVertexSize));
	GfUniquePtr<char[]> pFragSrc(LoadFileSrc("Shaders/bin/dummy.frag.spv", uiFragmentSize));

	// Define vertex format
	GfVertexDeclaration kVertexFormat;
	kVertexFormat.Init(nullptr, 0, 0, EVertexInputRate::PerVertex);

	// Prepare parameter layout
	GfShaderAccessMask uiAccessMask(EShaderStageFlags::Fragment);
	m_kParamLayout.DefineParameter(EParamaterSlotType::SampledImage, uiAccessMask, 0);
	m_kParamLayout.Create(m_kContext);

	// Define material
	m_kMaterialT.SetTopology(EPrimitiveTopology::TriList);
	m_kMaterialT.SetRasterState(GfRasterState());		// Use default RasterState
	m_kMaterialT.SetMSState(GfMultiSamplingState());	// Default MSState (disabled)
	m_kMaterialT.SetBlendState(GfBlendState());		// Default BlendState (disabled)
	m_kMaterialT.SetVertexFormat(kVertexFormat);
	m_kMaterialT.SetMaterialPass(&m_kRenderPass);
	m_kMaterialT.SetShaderData(EShaderStage::Vertex, "main", pVertexSrc.get(), uiVertexSize);
	m_kMaterialT.SetShaderData(EShaderStage::Fragment, "main", pFragSrc.get(), uiFragmentSize);
	m_kMaterialT.Create(m_kContext);

	// TODO: Enable the next line
	// m_kMaterialT.AssignLayout(0, m_kParamLayout);

	// Prepare uniform factory
	m_kUniformFactory.SetMaxAllocationsPerParamType(EParamaterSlotType::SampledImage, 1);
	m_kUniformFactory.SetMaxAllocatedParamSets(16);
	m_kUniformFactory.Create(m_kContext);

	// Prepare descriptor set
	m_kParamSet.Init(&m_kParamLayout);
	m_kParamSet.Create(m_kContext, m_kUniformFactory);
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::CreateResources()
{
	s32 siW, siH, siComp;
	GfUniquePtr<char[]> pTextureData = LoadTexture("Textures/uv_test.png", siW, siH, siComp);
	
	ETextureUsageBits::GfMask uiUsageMask(0);
	uiUsageMask.Set(ETextureUsageBits::Transfer_Dst | ETextureUsageBits::Sampled);
	m_kTesTexture.Init((u32)siW, (u32)siH, 1, ETextureFormat::R8G8B8A8_UNorm, uiUsageMask, GfTexture2D::EFlags::Tilable);
	m_kTesTexture.Create(m_kContext);

	// Load data into texture's memory
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::BindResourcesToParamSets()
{

}

////////////////////////////////////////////////////////////////////////////////
// EOF