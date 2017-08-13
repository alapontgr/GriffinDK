#include "TestApp.h"
#include "Rendering\RenderDefines.h"
#include "Rendering\VK_Material.h"

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

  create_render_pass();

  create_material();

  create_mesh();

  create_command_buffers();
}

void TestApp::on_update()
{

}

void TestApp::on_render()
{
  m_pRi->beginFrame();

  auto time = Framework::Time::total_time_secs();
  //m_context.test_render(time);


  m_pRi->endFrame();
}

void TestApp::on_release()
{

}



void TestApp::create_render_pass()
{
  m_renderPass.init(*m_pRenderInterface);
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

  Rendering::Material::MaterialDesc matDesc;

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
  m_mesh.init(reinterpret_cast<const f32*>(
    &g_vertices[0]), 4, sizeof(VertexDesc),
    g_indices, 6, sizeof(u16),
    m_mallocAllocator);
}

void TestApp::create_command_buffers()
{
  m_pCmdBuffer = Rendering::CommandBuffer::create(m_mallocAllocator);
}
