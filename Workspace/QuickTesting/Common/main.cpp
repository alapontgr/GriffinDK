#include "GfCore/Common/GfCore.h"
#include "GfMemory/Common/GfRAII.h"
#include "GfEntry/Common/GfEntry.h"
#include "GfRender/Common/GfMaterial.h"
#include "GfCore/Common/GfSingleton.h"
#include "GfRender/Common/GfRender.h"
#include "GfFile/Common/GfFile.h"

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

s32 _GfEntry_(const GfEntryArgs& kEntryParams) 
{
	// Create the window
	GfWindowInitParams kWindowInit;
	kWindowInit.m_uiWidth = 1280;
	kWindowInit.m_uiHeight = 720;
	kWindowInit.m_bVSync = false;
	kWindowInit.m_bFullScreen = false;
	kWindowInit.m_szWindowName = "TestGriffinApp";
	GfWindow kWindow;
	kWindow.Init(kWindowInit);

	// Init the render context
	GfRenderContext kContext;
	kContext.Init(&kWindow);

	// Create RenderPass and CmdBuffer
	GfRenderPass kRenderPass;
	kRenderPass.Compile(kContext);

	// Create material
	u32 uiVertexSize, uiFragmentSize;
	GfUniquePtr<char[]> pVertexSrc(LoadFileSrc("Shaders/bin/dummy.vert.spv", uiVertexSize));
	GfUniquePtr<char[]> pFragSrc(LoadFileSrc("Shaders/bin/dummy.frag.spv", uiFragmentSize));
	
	// Define vertex format
	GfVertexDeclaration kVertexFormat;
	kVertexFormat.Init(nullptr, 0, 0, EVertexInputRate::PerVertex);

	// Define material
	GfMaterialTemplate kMaterialT;
	kMaterialT.SetTopology(EPrimitiveTopology::TriList);
	kMaterialT.SetRasterState(GfRasterState());		// Use default RasterState
	kMaterialT.SetMSState(GfMultiSamplingState());	// Default MSState (disabled)
	kMaterialT.SetBlendState(GfBlendState());		// Default BlendState (disabled)
	kMaterialT.SetVertexFormat(kVertexFormat);
	kMaterialT.SetMaterialPass(&kRenderPass);
	kMaterialT.SetShaderData(EShaderStage::Vertex, "main", pVertexSrc.get(), uiVertexSize);
	kMaterialT.SetShaderData(EShaderStage::Fragment, "main", pFragSrc.get(), uiFragmentSize);
	kMaterialT.Create(kContext);
	
	GfCmdBufferFactory kCmdBufferFactory;
	kCmdBufferFactory.Init(kContext, GfRenderContextFamilies::Graphics);
	GfCmdBuffer pCmdBuffes[GfRenderConstants::ms_uiNBufferingCount];
	for (u32 i=0;i<GfRenderConstants::ms_uiNBufferingCount;++i)
	{
		kCmdBufferFactory.CreateCmdBuffer(kContext, GfCmdBufferType::Primary, pCmdBuffes[i]);
	}

	// Init Viewport and Scissor state
	GfViewport kViewport{};
	kViewport.m_fWidth = (f32)kWindow.GetWidth();
	kViewport.m_fHeight = (f32)kWindow.GetHeight();
	GfScissor kScissor{};
	kScissor.m_siWidth = (s32)kViewport.m_fWidth;
	kScissor.m_siHeight = (s32)kViewport.m_fHeight;

	while (kContext.BeginFrame())
	{
		u32 uiCurrFrameIdx(kContext.GetCurrentFrameIdx());
		GfCmdBuffer& kCmdBuffer(pCmdBuffes[uiCurrFrameIdx]);

		// Wait for the command buffer to be ready
		kCmdBuffer.WaitForReady(kContext);

		// Begin the Command buffer
		kCmdBuffer.BeginRecording(kContext);

		// Begin render pass
		kCmdBuffer.BeginRenderPass(kContext, kRenderPass);

		// Set Viewport and Scissor
		kCmdBuffer.SetViewport(kViewport);
		kCmdBuffer.SetScissor(kScissor);

		// Add commands
		kCmdBuffer.BindMaterial(kMaterialT);

		// Draw
		kCmdBuffer.Draw(3, 1);

		// End render pass
		kCmdBuffer.EndRenderPass(kContext, kRenderPass);

		// End the command buffer
		kCmdBuffer.EndRecording(kContext);

		// Submit command buffer
		kCmdBuffer.Submit(kContext, GfRenderContextFamilies::Present, GF_TRUE);

		kContext.EndFrame();
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////