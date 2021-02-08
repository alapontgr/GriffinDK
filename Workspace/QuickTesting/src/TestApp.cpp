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
	GfFile::OpenFile(GfPaths::getAssetPath(szFilepath).c_str(), EFileAccessMode::Read, kFile);
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
	init(kEntryParams);
	
	while (m_kWindow.BeginFrame(m_kContext))
	{
		Update();

		u32 uiCurrFrameIdx(m_kWindow.GetCurrentFrameIdx());
		GfCmdBuffer& kCmdBuffer(m_pCmdBuffes[uiCurrFrameIdx]);

		// Wait for the command buffer to be ready
		kCmdBuffer.waitForReady(m_kContext);

		// Begin the Command buffer
		kCmdBuffer.beginRecording(m_kContext);

		static bool pendingInitialization(true);
		if(pendingInitialization)
		{
			pendingInitialization = false;
			//DoTransferOperations(kCmdBuffer);
			GF_INFO("Create: %s", "Damaged helmet");
			m_damagedHelmet->create(m_kContext, kCmdBuffer);
			GF_INFO("After Create: %s", "Damaged helmet");
		}

		Render(kCmdBuffer);

		// End the command buffer
		kCmdBuffer.endRecording(m_kContext);

		// Submit command buffer
		kCmdBuffer.submit(m_kContext, m_kWindow, GfRenderContextFamilies::Present, GF_TRUE);

		m_kWindow.EndFrame(m_kContext);

		// Remove pending resources to delete
		GfBufferFactory::removePending(m_kContext);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::init(const GfEntryArgs& kEntryParams)
{
	GfCommandLine::init(kEntryParams.m_szCmdLine);
	GfPaths::init();

	// Create the window
	GfWindowInitParams kWindowInit;
	kWindowInit.m_width = 512;
	kWindowInit.m_height = 512;
	kWindowInit.m_bVSync = false;
	kWindowInit.m_bFullScreen = false;
	kWindowInit.m_szWindowName = "TestGriffinApp";
	m_kWindow.init(kWindowInit, m_kContext);

	GfInput::init();

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
	m_kCmdBufferFactory.init(m_kContext, GfRenderContextFamilies::Graphics);
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

	// Bind buffers
	GfBuffer* vertexBuffers[2] = 
	{
		GfBufferFactory::get(m_damagedHelmet->getVertexBuffer()),
		GfBufferFactory::get(m_damagedHelmet->getVertexBuffer())
	};
	u32 vertexBufferOffsets[] = { 0,0 };
	GfBuffer* indexBuffer(GfBufferFactory::get(m_damagedHelmet->getIndexBuffer()));
	u32 meshCount = m_damagedHelmet->getMeshCount();
	for (u32 i = 0; i < meshCount; ++i) 
	{
		const GfSubMesh& mesh = m_damagedHelmet->getMeshAt(i);
		vertexBufferOffsets[0] = mesh.getVertexBufferOffset(0);
		vertexBufferOffsets[1] = mesh.getVertexBufferOffset(1);
		kCmdBuffer.bindVertexBuffers(vertexBuffers, vertexBufferOffsets, 2);
		kCmdBuffer.bindIndexBuffer(*indexBuffer, mesh.getIndexBufferOffset(), mesh.doesUseShortForIndex());
		kCmdBuffer.drawIndexed(mesh.getIndexCount(), 1);
	}

	// Draw
	//kCmdBuffer.draw(6, 1);

	// End render pass
	m_kRenderPass.EndPass(kCmdBuffer);
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::Shutdown()
{
	GfInput::Shutdown();
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::CreateMaterialsAndParamSets()
{
	// Create material
	u32 uiVertexSize, uiFragmentSize;
	GfUniquePtr<char[]> pVertexSrc(LoadFileSrc("Shaders/bin/OpaqueTestVS.vert.spv", uiVertexSize));
	GfUniquePtr<char[]> pFragSrc(LoadFileSrc("Shaders/bin/OpaqueTestFS.frag.spv", uiFragmentSize));

	// Define vertex format
	GfVertexDeclaration kVertexFormat;
	GfVertexDeclaration::AttributeDesc vertexAttribs[4] =
	{
		{static_cast<u32>(offsetof(PosVertexBuffer, m_pos)),					0, 0, EAttributeFormat::SFloat3}, // Position
		{static_cast<u32>(offsetof(NormalsTangentUvVertexBuffer, m_normal)),	1, 1, EAttributeFormat::SFloat3}, // Normal
		{static_cast<u32>(offsetof(NormalsTangentUvVertexBuffer, m_tangent)),	2, 1, EAttributeFormat::SFloat3}, // Tangent
		{static_cast<u32>(offsetof(NormalsTangentUvVertexBuffer, m_uv)),		3, 1, EAttributeFormat::SFloat2}, // UVs
	};
	GfVertexDeclaration::VertexBufferBinding vertexBufferBindings[] = 
	{
		{0, static_cast<u32>(sizeof(PosVertexBuffer)), EVertexInputRate::PerVertex},
		{1, static_cast<u32>(sizeof(NormalsTangentUvVertexBuffer)), EVertexInputRate::PerVertex},
	};
	kVertexFormat.init(vertexAttribs, 4, vertexBufferBindings, 2);

	// Prepare parameter layout
	{
		GfShaderAccessMask uiAccessMask(EShaderStageFlags::Vertex);
		m_kParamLayout.DefineParameter(EParamaterSlotType::ConstantBuffer, uiAccessMask, 0);
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
	m_kParamSet.init(&m_kParamLayout);
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
	kDesc.m_mappable = true;
	m_kConstantBufferPool.init(kDesc);
	m_kConstantBufferPool.create(m_kContext);

	GfBuffer::GfRange kRange;
	kRange.m_pBuffer = &m_kConstantBufferPool;
	kRange.m_uiOffset = 0;
	kRange.m_uiSize = sizeof(GfPerFrameCB);
	m_kPerFrameCB.BindBuffer(kRange);

	////////////////////////////////////////////////////////////////////////////////
	// Bind

	m_kParamSet.BindResource(0, &m_kPerFrameCB);
	m_kParamSet.Update(m_kContext);
}

void TestApp::loadGLTF()
{
	m_damagedHelmet = GfGltfLoader::Load("./Models/DamagedHelmet/glTF/DamagedHelmet.gltf");
}

////////////////////////////////////////////////////////////////////////////////
// EOF