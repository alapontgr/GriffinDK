#include "GfCore/Common/GfCore.h"
#include "GfEntry/Common/GfEntry.h"
#include "GfRender/Common/GfRender.h"

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

	GfCmdBuffer kCmdBuffer;
	GfCmdBufferFactory kCmdBufferFactory;
	kCmdBufferFactory.Init(kContext, GfRencerContextFamilies::Graphics);
	kCmdBufferFactory.CreateCmdBuffer(kContext, GfCmdBufferType::Primary, kCmdBuffer);

	while (kContext.BeginFrame())
	{
		// Wait for the command buffer to be ready
		kCmdBuffer.WaitForReady(kContext);

		// Begin the Command buffer
		kCmdBuffer.BeginRecording(kContext);

		// Begin render pass
		kCmdBuffer.BeginRenderPass(kContext, kRenderPass);

		// End render pass
		kCmdBuffer.BeginRenderPass(kContext, kRenderPass);

		// End the command buffer
		kCmdBuffer.EndRecording(kContext);

		// Submit command buffer
		kCmdBuffer.Submit(kContext, GfRencerContextFamilies::Present, GF_TRUE);

		kContext.EndFrame();
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////