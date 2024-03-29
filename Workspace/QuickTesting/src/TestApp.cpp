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
#include "Common/GfShaderCompiler/GfShaderCompiler.h"

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
		GfUniquePtr<char[]> pSrc(new char[uiOutFileSize+1]);
		pSrc[uiOutFileSize] = 0;
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
	
	while (m_window.beginFrame(m_context))
	{
		Update();

		u32 uiCurrFrameIdx(m_window.getCurrentFrameIdx());
		GfCmdBuffer* cmdBuffer = GfCmdBuffer::get(&m_context, GfCmdBufferType::Primary, GfRenderContextFamilies::Graphics);

		Render(*cmdBuffer);

		// Final submit
		const GfSemaphore& imageReady = m_window.getImageReadySemaphore();
		const GfSemaphore& signalSemaphore = m_window.getFinishedRenderingSemaphore();
		cmdBuffer->submit(&imageReady, &signalSemaphore);

		m_context.tick();
		m_window.endFrame(m_context);

		// Remove pending resources to delete
		GfBufferFactory::removePending(m_context);
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
	m_window.init(kWindowInit, m_context);

	GfInput::init();

	////////////////////////////////////////////////////////////////////////////////

	GfString shaderCache = GfCommandLine::getArg(GfHash::compute("-sc"), ".");
	GfString testShader = GfCommandLine::getArg(GfHash::compute("-f"));

	if (shaderCache.size() == 0) 
	{
		GF_ASSERT_ALWAYS("Need to provide path for shader cache");
	}

	if (testShader.size() == 0) 
	{
		GF_ASSERT_ALWAYS("Need to provide path for shader to test");
	}

	u32 srcSize = 0;
	GfUniquePtr<char[]> src = LoadFileSrc(testShader.c_str(), srcSize);
	GfString shaderSrc(src.get());
	std::replace( shaderSrc.begin(), shaderSrc.end(), '\\', '/');
	if (shaderSrc.find("\r\n") != GfString::npos) 
	{
		shaderSrc.erase(std::remove(shaderSrc.begin(), shaderSrc.end(), '\r'), shaderSrc.end());
	}

	GfString errors;
	m_shaderCache.init(shaderCache);
	GfShaderCompiler compiler;
	compiler.init("", "");
	compiler.compileShader(testShader.c_str(), shaderSrc, m_shaderCache, errors);

	if (errors.size() > 0) 
	{
		GF_ERROR("%s", errors.c_str());
	}

	u32 shaderBlobSize(0);
	m_shader.setShaderBlob(m_shaderCache.getShaderBlob(testShader));


	////////////////////////////////////////////////////////////////////////////////

	m_kCamera.UpdatePerspective(GF_DEG_TO_RAD(45.0f), (f32)m_window.getWidth() / (f32)m_window.getHeight(), 0.1f, 1000.0f);
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

void TestApp::Render(GfCmdBuffer& cmdBuffer)
{
	f32 width = static_cast<f32>(m_window.getWidth());
	f32 height = static_cast<f32>(m_window.getHeight());

	GfTextureView backBufferView(m_window.getCurrBackBuffer());
	static constexpr u32 s_AttachmentCount = 1;
	AttachmentDesc attachments[] = 
	{
		{GfTextureView(m_window.getCurrBackBuffer()), LoadOp::Clear, StoreOp::Store} 
	};
	m_renderPass.setAttachments(attachments, s_AttachmentCount, nullptr);
	m_renderPass.setViewport(width, height, 0.0f, 0.0f);
	m_renderPass.setScissor(m_window.getWidth(), m_window.getHeight(), 0, 0);
	m_renderPass.setRenderArea(m_window.getWidth(), m_window.getHeight(), 0, 0);

	u32 curFrameIdx(m_context.getCurFrame());

	m_renderPass.setClearColor(v4(
		static_cast<f32>(curFrameIdx & 0xff) / 255.0f, 
		static_cast<f32>((curFrameIdx>>8) & 0xff) / 255.0f,
		static_cast<f32>((curFrameIdx>>16) & 0xff) / 255.0f,
		0.0f));

	// Recording

	cmdBuffer.beginRecording();

	// Transit swapchain to Color attachment
	if (m_context.getCurFrame() == 0) 
	{
		cmdBuffer.addTextureBarrier(attachments[0].m_attachment, TextureUsageFlags::Undefined, TextureUsageFlags::ColorAttachment);
	}
	else 
	{
		cmdBuffer.addTextureBarrier(attachments[0].m_attachment, TextureUsageFlags::Present, TextureUsageFlags::ColorAttachment);
	}

	cmdBuffer.beginRenderPass(&m_renderPass);

	// Render
	GfShaderVariant variant(&m_shader);
	cmdBuffer.bindShaderPipe(variant);
	cmdBuffer.draw(3, 1);


	cmdBuffer.endRenderPass();

	// Transit to present
	cmdBuffer.addTextureBarrier(attachments[0].m_attachment, TextureUsageFlags::ColorAttachment, TextureUsageFlags::Present);

	cmdBuffer.endRecording();
}

////////////////////////////////////////////////////////////////////////////////

void TestApp::Shutdown()
{
	GfInput::Shutdown();
}

////////////////////////////////////////////////////////////////////////////////
// EOF