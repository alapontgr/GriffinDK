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

#include "./src/TestApp.h"
#include "Common/GfInput/GfInput.h"

#include "stb/stb_image.h"

#include "GfMesh.h"

////////////////////////////////////////////////////////////////////////////////

struct GfPerFrameCB 
{
	m4 m_mView;
	m4 m_mProjection;
	m4 m_mViewProjection;
} g_kPerFrameCBData;

////////////////////////////////////////////////////////////////////////////////

static GfUniquePtr<char[]> LoadFileSrc(const char* szFilepath, u32& uiOutFileSize)
{
	GfFileHandle kFile;
	GfFile::OpenFile(szFilepath, EFileAccessMode::Read, kFile);
	GfFile::GetFileSize(kFile);
	uiOutFileSize = static_cast<u32>(kFile.GetFileSize());
	if (uiOutFileSize > 0) 
	{
		GfUniquePtr<char[]> pSrc(new char[uiOutFileSize]);
		u32 uiRead = GfFile::ReadBytes(kFile, uiOutFileSize, pSrc.get());
		GfFile::CloseFile(kFile);
		GF_ASSERT(uiRead == uiOutFileSize, "Invalid size read");
		return pSrc;
	}
	return nullptr;
}

static GfUniquePtr<char[]> LoadFileBin(const char* szFilepath, u32& uiOutFileSize) 
{
	GfFileHandle kFile;
	GfFile::OpenFile(szFilepath, EFileAccessMode::Read, kFile);
	GfFile::GetFileSize(kFile);
	uiOutFileSize = static_cast<u32>(kFile.GetFileSize());
	if (uiOutFileSize > 0) 
	{
		GfUniquePtr<char[]> pSrc(new char[uiOutFileSize]);
		u32 uiRead = GfFile::ReadBytes(kFile, uiOutFileSize, pSrc.get());
		GfFile::CloseFile(kFile);
		GF_ASSERT(uiRead == uiOutFileSize, "Invalid size read");
		return pSrc;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

static bool doesFileExist(const std::string &abs_filename, void *) 
{
	return GfFile::DoesFileExist(abs_filename.c_str());
}

static GfString expandFilePath(const std::string & relPath, void *) 
{
	return GfPaths::getAssetPath(relPath);
}

static bool readWholeFile(std::vector<unsigned char>* outBuffer, std::string* error, const std::string & filePath, void *)
{
	GfFileHandle kFile;
	GfFile::OpenFile(filePath.c_str(), EFileAccessMode::Read, kFile);
	GfFile::GetFileSize(kFile);
	u32 fileSize = static_cast<u32>(kFile.GetFileSize());
	if (fileSize > 0) 
	{
		outBuffer->resize(fileSize);
		u32 uiRead = GfFile::ReadBytes(kFile, fileSize, outBuffer->data());
		GfFile::CloseFile(kFile);
		if (uiRead == fileSize) 
		{
			return true;
		}
		if (error) 
		{
			*error = "Failed to read file: " + filePath;
		}
	}
	return false;
}

static bool writeWholeFile(std::string * error, const std::string & filePath, 
	const std::vector<unsigned char> & data, void * userData)
{
	if (data.size() > 0) 
	{
		GfFileHandle kFile;
		GfFile::OpenFile(filePath.c_str(), EFileAccessMode::Write, kFile);
		u32 fileSize = static_cast<u32>(kFile.GetFileSize());
		u32 uiWriten = GfFile::WriteBytes(kFile, u32(data.size()), reinterpret_cast<const void*>(data.data()));
		GfFile::CloseFile(kFile);
		if (uiWriten == u32(data.size())) 
		{
			return true;
		}
		if (error) 
		{
			*error = "Failed to write to file: " + filePath;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

GfUniquePtr<char[]> LoadTexture(const char* szTexturePath, s32& siWidth, s32& siHeight, s32& siComponentCount) 
{
	// Force 4 components
	u32 uiReqComponents(4);
	unsigned char *pData = stbi_load(szTexturePath, &siWidth, &siHeight, &siComponentCount, uiReqComponents);
	size_t uiTextureSize((size_t)siWidth * siHeight * uiReqComponents);
	
	GfUniquePtr<char[]> pSrc(new char[uiTextureSize]);
	memcpy(pSrc.get(), pData, uiTextureSize);
	stbi_image_free(pData);

	return pSrc;
}

////////////////////////////////////////////////////////////////////////////////

s32 TestApp::Run(const GfEntryArgs& kEntryParams)
{
	Init(kEntryParams);
	
	while (m_kWindow.BeginFrame(m_kContext))
	{
		Update();

		u32 uiCurrFrameIdx(m_kWindow.GetCurrentFrameIdx());
		GfCmdBuffer& kCmdBuffer(m_pCmdBuffes[uiCurrFrameIdx]);

		// Wait for the command buffer to be ready
		kCmdBuffer.WaitForReady(m_kContext);

		// Begin the Command buffer
		kCmdBuffer.BeginRecording(m_kContext);

		static bool bPendingTransfer(true);
		if(bPendingTransfer)
		{
			bPendingTransfer = false;
			DoTransferOperations(kCmdBuffer);
		}

		Render(kCmdBuffer);

		// End the command buffer
		kCmdBuffer.EndRecording(m_kContext);

		// Submit command buffer
		kCmdBuffer.Submit(m_kContext, m_kWindow, GfRenderContextFamilies::Present, GF_TRUE);

		m_kWindow.EndFrame(m_kContext);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::Init(const GfEntryArgs& kEntryParams)
{
	GfCommandLine::init(kEntryParams.m_szCmdLine);
	GfPaths::Init();

	// Create the window
	GfWindowInitParams kWindowInit;
	kWindowInit.m_uiWidth = 512;
	kWindowInit.m_uiHeight = 512;
	kWindowInit.m_bVSync = false;
	kWindowInit.m_bFullScreen = false;
	kWindowInit.m_szWindowName = "TestGriffinApp";
	m_kWindow.Init(kWindowInit, m_kContext);

	GfInput::Init();

	////////////////////////////////////////////////////////////////////////////////

	// Create resources
	CreateRenderPasses();
	CreateMaterialsAndParamSets();
	CreateCmdBuffers();
	CreateResources();

	m_kCamera.UpdatePerspective(GF_DEG_TO_RAD(45.0f), (f32)m_kWindow.GetWidth() / (f32)m_kWindow.GetHeight(), 0.1f, 1000.0f);
	m_kCamera.UpdateViewWithTarget(v3(0.0f, 5.0f, 10.0f), v3(0.0f), v3(0.0f, 1.0f, 0.0f));
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::Update()
{
	static f64 s_dLastFrameTime(GfTime::GetTimeInSeconds());
	f64 dCurrTime = GfTime::GetTimeInSeconds();
	f32 fElapsedTime = (f32)(dCurrTime - s_dLastFrameTime);
	s_dLastFrameTime = dCurrTime;

	GfInput::Update();

	static f32 s_fDeg(0.0f);

	s_fDeg += fElapsedTime * 45.0f;
	if (s_fDeg > 360.0f) 
	{
		s_fDeg -= 360.0f;
	}

	f32 fSin(sinf(GF_DEG_TO_RAD(s_fDeg)));
	f32 fCos(cosf(GF_DEG_TO_RAD(s_fDeg)));

	fCos = fCos * 0.5f + 0.5f;

	m_kCamera.UpdateViewWithTarget(v3(fSin * 3.0f, 0.0f, fCos * 5.0f + 2.0f), v3(0.0f), v3(0.0f, 1.0f, 0.0f));
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
	m_kRenderPass.Create(m_kContext, &m_kWindow);

	// Init Viewport and Scissor state
	m_kViewport.m_fWidth = (f32)m_kWindow.GetWidth();
	m_kViewport.m_fHeight = (f32)m_kWindow.GetHeight();

	m_kScissor.m_siWidth = (s32)m_kViewport.m_fWidth;
	m_kScissor.m_siHeight = (s32)m_kViewport.m_fHeight;
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::Render(GfCmdBuffer& kCmdBuffer)
{
	// Update PerFrameCB
	g_kPerFrameCBData.m_mView = m_kCamera.GetView(); // Identity
	g_kPerFrameCBData.m_mProjection = m_kCamera.GetProjection(); // Identity
	g_kPerFrameCBData.m_mViewProjection = (g_kPerFrameCBData.m_mProjection * g_kPerFrameCBData.m_mView);

	GfPerFrameCB* pData = m_kPerFrameCB.MapAs<GfPerFrameCB>(m_kContext);
	if (pData) 
	{
		*pData = g_kPerFrameCBData;
		m_kPerFrameCB.UnMap(m_kContext);
	}

	// Begin render pass
	m_kRenderPass.BeginPass(m_kContext, kCmdBuffer, &m_kWindow);

	// Set Viewport and Scissor
	m_kRenderPass.SetViewport(kCmdBuffer, m_kViewport);
	m_kRenderPass.SetScissor(kCmdBuffer, m_kScissor);

	// Add commands
	m_kMaterialT.Bind(kCmdBuffer);
	m_kParamSet.BindSet(kCmdBuffer, m_kMaterialT, 0);

	// Draw
	kCmdBuffer.Draw(6, 1);

	// End render pass
	m_kRenderPass.EndPass(kCmdBuffer);
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::Shutdown()
{
	GfInput::Shutdown();
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::DoTransferOperations(GfCmdBuffer& kCmdBuffer)
{
	u32 uiTextureSize(m_kTesTexture.GetWidth() * m_kTesTexture.GetHeight() * 4);
	void* pData = m_kStagingBuffer.map(m_kContext, 0, uiTextureSize);
	if(pData)
	{
		memcpy(pData, m_pTestTextureData.get(), uiTextureSize);
		m_kStagingBuffer.unMap(m_kContext);
	}

	m_kTesTexture.LoadTexture2DDataFromStaging(m_kContext, kCmdBuffer, m_kStagingBuffer, 0);
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
	{
		GfShaderAccessMask uiAccessMask(EShaderStageFlags::Vertex);
		m_kParamLayout.DefineParameter(EParamaterSlotType::ConstantBuffer, uiAccessMask, 0);
	}
	{
		GfShaderAccessMask uiAccessMask(EShaderStageFlags::Fragment);
		m_kParamLayout.DefineParameter(EParamaterSlotType::CombinedTextureSampler, uiAccessMask, 1);
	}
	m_kParamLayout.Create(m_kContext);

	// Define material
	if (uiVertexSize && uiFragmentSize) 
	{
		m_kMaterialT.SetTopology(EPrimitiveTopology::TriList);
		m_kMaterialT.SetRasterState(GfRasterState());		// Use default RasterState
		m_kMaterialT.SetMSState(GfMultiSamplingState());	// Default MSState (disabled)
		m_kMaterialT.SetBlendState(GfBlendState());		// Default BlendState (disabled)
		m_kMaterialT.SetVertexFormat(kVertexFormat);
		m_kMaterialT.SetMaterialPass(&m_kRenderPass);
		m_kMaterialT.SetShaderData(EShaderStage::Vertex, "main", pVertexSrc.get(), uiVertexSize);
		m_kMaterialT.SetShaderData(EShaderStage::Fragment, "main", pFragSrc.get(), uiFragmentSize);
		m_kMaterialT.AssignLayout(0, &m_kParamLayout);
		m_kMaterialT.Create(m_kContext);
	}

	// Prepare uniform factory
	m_kUniformFactory.SetMaxAllocationsPerParamType(EParamaterSlotType::ConstantBuffer, 1);
	m_kUniformFactory.SetMaxAllocationsPerParamType(EParamaterSlotType::CombinedTextureSampler, 1);
	m_kUniformFactory.SetMaxAllocatedParamSets(16);
	m_kUniformFactory.Create(m_kContext);

	// Prepare descriptor set
	m_kParamSet.Init(&m_kParamLayout);
	m_kParamSet.Create(m_kContext, m_kUniformFactory);
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::CreateResources()
{
	loadGLTF();

	////////////////////////////////////////////////////////////////////////////////

	// Create staging buffer
	GfBuffer::GfBufferDesc kDesc;
	kDesc.m_size = 16 << 20; // 16MB
	kDesc.m_alignment = 16;
	kDesc.m_bufferType = BufferType::Staging;
	m_kStagingBuffer.init(kDesc);
	m_kStagingBuffer.create(m_kContext);

	////////////////////////////////////////////////////////////////////////////////
	// Init constant buffer pool

	kDesc.m_size = sizeof(GfPerFrameCB);
	kDesc.m_alignment = 16;
	kDesc.m_bufferType = BufferType::Uniform;
	m_kConstantBufferPool.init(kDesc);
	m_kConstantBufferPool.create(m_kContext);

	GfBuffer::GfRange kRange;
	kRange.m_pBuffer = &m_kConstantBufferPool;
	kRange.m_uiOffset = 0;
	kRange.m_uiSize = sizeof(GfPerFrameCB);
	m_kPerFrameCB.BindBuffer(kRange);

	////////////////////////////////////////////////////////////////////////////////
	// Init texture
	s32 siW, siH, siComp;
	m_pTestTextureData = LoadTexture("Textures/uv_test.png", siW, siH, siComp);
	
	ETextureUsageBits::GfMask uiUsageMask(0);
	uiUsageMask.Set(ETextureUsageBits::Transfer_Dst | ETextureUsageBits::Sampled);
	m_kTesTexture.Init((u32)siW, (u32)siH, 1, ETextureFormat::R8G8B8A8_UNorm, uiUsageMask, GfTexture2D::ETexture2DFlags::Tilable);
	m_kTesTexture.Create(m_kContext);

	// Create view for the texture
	m_kTestTexView.Init(&m_kTesTexture, ETextureViewType::View2D);
	m_kTestTexView.Create(m_kContext);

	////////////////////////////////////////////////////////////////////////////////
	// Sampler

	m_kSampler.SetAddrU(ETexAddressMode::Repeat);
	m_kSampler.SetAddrV(ETexAddressMode::Repeat);
	m_kSampler.SetAddrW(ETexAddressMode::Repeat);
	m_kSampler.Create(m_kContext);

	////////////////////////////////////////////////////////////////////////////////
	// Bind

	m_kCombinedSamplerTexture.Init(&m_kSampler, &m_kTestTexView);

	m_kParamSet.BindResource(0, &m_kPerFrameCB);
	m_kParamSet.BindResource(1, &m_kCombinedSamplerTexture);
	m_kParamSet.Update(m_kContext);
}

void TestApp::loadGLTF()
{
	GfGltfLoader::Load("./Models/DamagedHelmet/glTF/DamagedHelmet.gltf");
}

////////////////////////////////////////////////////////////////////////////////
// EOF