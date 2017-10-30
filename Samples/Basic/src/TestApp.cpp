#include "TestApp.h"
#include "Rendering\RenderInterface.h"
#include "Rendering\RenderConstants.h"

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

static constexpr u32 kTmpStackAllocatorSize = 1024 * 1024; // 1MB
static constexpr u32 kResourceStackAllocatorSize = 1024 * 1024; // 1MB

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
  m_frameResourceFactory.init(Rendering::kFrameBufferingCount);

  // Initialize the allocators
  m_tmpStackAllocator.init(m_mallocAllocator, kTmpStackAllocatorSize, 16);
  m_resStackAllocator.init(m_mallocAllocator, kTmpStackAllocatorSize, 16);

  create_render_pass();

  create_material();

  create_mesh();
}

void TestApp::on_update()
{

}

void TestApp::on_render()
{
  Rendering::RenderInterface::beginFrame();

  m_frameResourceFactory.get_current_fence()->wait();

  auto* pCmdBuffer = m_frameResourceFactory.get_primary_command_buffer(m_mallocAllocator);
  pCmdBuffer->start_recording();


  auto time = Framework::Time::total_time_secs();
  //m_context.test_render(time);

  // Update the surface to use to draw
  m_renderSurface.m_width = static_cast<u32>(width());
  m_renderSurface.m_height = static_cast<u32>(height());
  m_renderSurface.m_pColorTexture = Rendering::RenderInterface::get_screen();

  m_cmdList.clear();
  if (m_meshDirty) 
  {
    m_meshDirty = false;
    update_assets(*pCmdBuffer);
  }
  draw_geometry(*pCmdBuffer);

  pCmdBuffer->end_recording();

  submit_work(*pCmdBuffer);


  Rendering::RenderInterface::endFrame();

  m_frameResourceFactory.flip();
}

void TestApp::on_release()
{

}



void TestApp::create_render_pass()
{
  m_renderPass.init();
  m_frameResourceFactory.create_render_pass_resources(&m_renderPass);
}

void TestApp::create_material()
{
  initialize_param_set();

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

  Rendering::MaterialDesc matDesc;
  matDesc.m_pRenderPass = &m_renderPass;
  matDesc.m_vsPath = "Shaders/bin/dummy.vert.spv";
  matDesc.m_psPath = "Shaders/bin/dummy.frag.spv";
  matDesc.m_pVertexLayout = nullptr;
  matDesc.m_pBlendState = &m_blendState;
  matDesc.m_pRasterState = &m_rasterState;
  matDesc.m_pMultiSamplingState = &m_multiSampleState;

  m_material.init(matDesc, &m_parameterSet, 1);

  // Create the GPU Material
  m_material.create_material();
}

void TestApp::init_constant_buffer()
{

}

void TestApp::initialize_param_set()
{
  static Utilities::Name cbParamName("test_cb");
  Rendering::ShaderStageMask stagesMask = Rendering::EShaderStageFlag::Fragment;
  m_parameterSet.add_parameter(0, cbParamName, Rendering::Type_ConstantBuffer, stagesMask);
  m_parameterSet.create();

  m_paramBufferGroup.create();

  m_parameterBuffer.create(m_parameterSet, m_paramBufferGroup, m_resStackAllocator);

  auto pLayout = m_parameterBuffer.layout();
  auto slot = pLayout->get_parameter(cbParamName);
  m_parameterBuffer.set_parameter(slot, &m_testCB);

  init_constant_buffer();

  // Once all the resources of the parameter set are ready we can update it
  //m_parameterBuffer.update(m_tmpStackAllocator);
}

void TestApp::create_mesh()
{
  Rendering::BufferDesc desc;
  
  desc.m_size = 1024 * 1024 * 16; // 16 MB
  desc.m_currentUsage = Rendering::Transfer_Src;
  desc.m_alignment = 16;
  desc.m_bufferUsage = Rendering::EBufferUsage::Transfer_Src;
  desc.m_memoryProperties = Rendering::EMemoryProperties::CPU_Visible | Rendering::EMemoryProperties::CPU_GPU_Coherent;

  m_pStagingBuffer = Rendering::Buffer::create(m_mallocAllocator);
  m_pStagingBuffer->init(desc, m_mallocAllocator);

  m_mesh.init(reinterpret_cast<const f32*>(
    &g_vertices[0]), 4, sizeof(VertexDesc),
    g_indices, 6, sizeof(u16),
    m_mallocAllocator);

  Rendering::RenderInterface::create_buffer(*m_pStagingBuffer);
  Rendering::RenderInterface::create_buffer(*m_mesh.index_buffer());
  Rendering::RenderInterface::create_buffer(*m_mesh.vertex_buffer());

  u32 sizeVertexBuffer = 4 * sizeof(VertexDesc);
  u32 sizeIndexBuffer = 6 * sizeof(16);
  u32 totalSize = sizeVertexBuffer + sizeIndexBuffer;

  Memory::mem_t* pData = static_cast<Memory::mem_t*>(
    Rendering::RenderInterface::map_buffer_gpu_memory(*m_pStagingBuffer, 0, totalSize));

  memcpy(pData, reinterpret_cast<const void*>(&g_vertices[0]), sizeVertexBuffer);
  memcpy((pData + sizeVertexBuffer), g_indices, sizeIndexBuffer);

  Rendering::RenderInterface::unmap_buffer_gpu_memory(*m_pStagingBuffer);

  m_meshDirty = true;
}

void TestApp::update_assets(Rendering::CommandBuffer& rCmdBuffer)
{
  u32 sizeVertexBuffer = 4 * sizeof(VertexDesc);
  u32 sizeIndexBuffer = 6 * sizeof(u16);
  u32 totalSize = sizeVertexBuffer + sizeIndexBuffer;
  auto* pVertexBuffer = m_mesh.vertex_buffer();
  auto* pIndexBuffer = m_mesh.index_buffer();

  pVertexBuffer->copy_range_from(*m_pStagingBuffer, 0, 0, sizeVertexBuffer, rCmdBuffer);
  pIndexBuffer->copy_range_from(*m_pStagingBuffer, sizeVertexBuffer, 0, sizeIndexBuffer, rCmdBuffer);
}

void TestApp::draw_geometry(Rendering::CommandBuffer& rCmdBuffer)
{
  auto* pFramebuffer = m_frameResourceFactory.get_render_pass_framebuffer(&m_renderPass);
  pFramebuffer->destroy();
  pFramebuffer->create(m_renderPass, m_renderSurface);

  m_renderPass.start(rCmdBuffer, *pFramebuffer);
  // Draw
  Rendering::Viewport viewport;
  viewport.m_dimensions = v2(width(), height());
  viewport.m_offset = v2(0.0f);
  viewport.m_minDepth = 0.0f;
  viewport.m_maxDepth = 1.0f;
  Rendering::Scissor scissor;
  scissor.m_width = pFramebuffer->width();
  scissor.m_height = pFramebuffer->height();
  scissor.m_offsetX = 0;
  scissor.m_offsetY = 0;
  Rendering::RenderInterface::set_scissor(scissor, rCmdBuffer);
  Rendering::RenderInterface::set_viewport(viewport, rCmdBuffer);

  Rendering::RenderInterface::bind_material(m_material, rCmdBuffer);
  Rendering::RenderInterface::use_mesh(m_mesh, rCmdBuffer);
  Rendering::RenderInterface::draw_indexed(6, 1, 0, 0, rCmdBuffer);

  m_renderPass.end(rCmdBuffer);
}

void TestApp::submit_work(Rendering::CommandBuffer& rCmdBuffer)
{
  Rendering::RenderInterface::submit_work(rCmdBuffer, *m_frameResourceFactory.get_current_fence());
}
