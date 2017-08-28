#include "TestApp.h"
#include "Rendering\VK_Material.h"
#include "Rendering\context.h"
#include "Rendering\Framebuffer.h"

struct VertexDesc // Temporary
{
  v4 position;
  v4 colour;
};

static const VertexDesc g_vertices[] =
{
  { v4(-0.5f, -0.5f, 0.0f, 1.0f),
  v4(1.0f, 0.0f, 0.0f, 1.0f) },
  { v4(0.5f, -0.5f, 0.0f, 1.0f),
  v4(1.0f, 1.0f, 0.0f, 1.0f) },
  { v4(0.5f, 0.5f, 0.0f, 1.0f),
  v4(0.0f, 1.0f, 1.0f, 1.0f) },
  { v4(-0.5f, 0.5f, 0.0f, 1.0f),
  v4(0.0f, 0.0f, 1.0f, 1.0f) },
};

static const u16 g_indices[] = { 0, 1, 2, 2, 3, 0 };

TestApp::TestApp()
{

}

TestApp::~TestApp()
{

}

void TestApp::on_pre_init()
{
  s_width = 512;//1280;
  s_height = 512;//720;
}

void TestApp::on_start()
{
  // Use the Vulkan render interface
  m_pRi = reinterpret_cast<Rendering::VK_RenderInterface*>(m_pRenderInterface);

  m_frameResourceFactory.init(Rendering::kBufferCount, *m_pRenderInterface);

  create_render_pass();

  create_material();

  create_mesh();
}

void TestApp::on_update()
{

}

void TestApp::on_render()
{
  m_pRi->beginFrame();

  m_frameResourceFactory.get_current_fence()->wait(*m_pRenderInterface);

  auto* pCmdBuffer = m_frameResourceFactory.get_primary_command_buffer(*m_pRenderInterface, m_mallocAllocator);
  pCmdBuffer->start_recording();


  auto time = Framework::Time::total_time_secs();
  //m_context.test_render(time);

  // Update the surface to use to draw
  m_renderSurface.m_width = static_cast<u32>(width());
  m_renderSurface.m_height = static_cast<u32>(height());
  m_renderSurface.m_pColorTexture = m_pRenderInterface->get_screen();

  m_cmdList.clear();
  if (m_meshDirty) 
  {
    m_meshDirty = false;
    update_assets(*pCmdBuffer);
  }
  draw_geometry(*pCmdBuffer);

  pCmdBuffer->end_recording();

  submit_work(*pCmdBuffer);


  m_pRi->endFrame();

  m_frameResourceFactory.flip();
}

void TestApp::on_release()
{

}



void TestApp::create_render_pass()
{
  m_renderPass.init(*m_pRenderInterface);
  m_frameResourceFactory.create_render_pass_resources(&m_renderPass);
}

void TestApp::create_material()
{
  m_blendState.m_enabledBlending = false;
  m_blendState.m_enableLogicOperations = false;
  m_blendState.m_srcColorBlendFactor = Rendering::BlendState::One;
  m_blendState.m_dstColorBlendFactor = Rendering::BlendState::Zero;
  m_blendState.m_colorBlendOP = Rendering::BlendState::Add;
  m_blendState.m_srcAlphaBlendFactor = Rendering::BlendState::One;
  m_blendState.m_dstAlphaBlendFactor = Rendering::BlendState::Zero;
  m_blendState.m_alphaBlendOP = Rendering::BlendState::Add;
  m_blendState.m_logicOP = Rendering::BlendState::Copy;
  m_blendState.m_blendConstants = v4(0, 0, 0, 0);

  m_rasterState.m_enableRasterDiscard = false;
  m_rasterState.m_cullMode = Rendering::RasterState::Back;
  m_rasterState.m_polygonMode = Rendering::RasterState::Fill;

  m_multiSampleState.m_enabled = false;
  m_multiSampleState.m_samplesCount = Rendering::MultiSampleState::x1;

  m_pMaterial = Rendering::Material::create(m_mallocAllocator);

  Rendering::MaterialDesc matDesc;

  matDesc.m_pRenderPass = &m_renderPass;
  matDesc.m_vsPath = "Shaders/bin/dummy.vert.spv";
  matDesc.m_psPath = "Shaders/bin/dummy.frag.spv";
  matDesc.m_pVertexLayout = nullptr;
  matDesc.m_pBlendState = &m_blendState;
  matDesc.m_pRasterState = &m_rasterState;
  matDesc.m_pMultiSamplingState = &m_multiSampleState;

  m_pMaterial->init(matDesc);

  // Create the GPU Material
  m_pMaterial->create_material(*m_pRenderInterface);
}

void TestApp::create_mesh()
{
  Rendering::Buffer::BufferDesc desc;
  desc.m_size = 1024 * 1024 * 16; // 16 MB
  desc.m_alignment = 16;
  desc.m_bufferUsage = Rendering::Buffer::EBufferUsage::Transfer_Src;
  desc.m_memoryProperties = Rendering::Buffer::EMemoryProperties::CPU_Visible | Rendering::Buffer::EMemoryProperties::CPU_GPU_Coherent;

  m_pStagingBuffer = Rendering::Buffer::create(m_mallocAllocator);
  m_pStagingBuffer->init(desc, m_mallocAllocator);

  m_mesh.init(reinterpret_cast<const f32*>(
    &g_vertices[0]), 4, sizeof(VertexDesc),
    g_indices, 6, sizeof(u16),
    m_mallocAllocator);

  m_pRenderInterface->create_buffer(*m_pStagingBuffer);
  m_pRenderInterface->create_buffer(*m_mesh.index_buffer());
  m_pRenderInterface->create_buffer(*m_mesh.vertex_buffer());

  // TODO: Map staging buffer and upload the data of the vertex buffer
  // TODO: Map the staging buffer and upload the data of the vertex buffer

  // TODO: Copy region from the staging buffer to the vertex buffer
  // TODO: Copy region from the staging buffer to the index buffer

  m_meshDirty = true;
}

void TestApp::update_assets(Rendering::CommandBuffer& rCmdBuffer)
{
  // TODO: Update vertex and index buffers
}

void TestApp::draw_geometry(Rendering::CommandBuffer& rCmdBuffer)
{
  auto* pFramebuffer = m_frameResourceFactory.get_render_pass_framebuffer(&m_renderPass);
  pFramebuffer->destroy(*m_pRenderInterface);
  pFramebuffer->create(*m_pRenderInterface, m_renderPass, m_renderSurface);

  m_renderPass.start(*m_pRenderInterface, rCmdBuffer, *pFramebuffer);

  // Draw

  m_renderPass.end(*m_pRenderInterface, rCmdBuffer);
}

void TestApp::submit_work(Rendering::CommandBuffer& rCmdBuffer)
{
  m_pRenderInterface->submit_work(rCmdBuffer, *m_frameResourceFactory.get_current_fence());
}
