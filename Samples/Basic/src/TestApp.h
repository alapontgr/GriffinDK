#pragma once

#include "Entry.h"

#include "Utilities/types.h" 
#include "Memory/MallocAllocator.h"
#include "Memory/StackAllocator.h"

#include "Framework/app.h"
#include "Framework/Time.h"
#include "Framework/Mesh.h"

#include "Rendering/CommandBuffer.h"
#include "Rendering/RenderConfiguration.h"
#include "Rendering/Material.h"
#include "Rendering/Buffer.h"
#include "Rendering/RenderSurface.h"
#include "Rendering/FrameResourceFactory.h"
#include "Rendering/MaterialParameterSet.h"
#include "Rendering/ParameterBuffer.h"
#include "Rendering/ParameterBufferGroup.h"
#include "Rendering/ConstantBuffer.h"



using namespace fdk;

struct TestCB
{
  v4 m_colour;
};

class TestApp : public fdk::Framework::App
{
public:
  TestApp();
  virtual ~TestApp();

  virtual void on_pre_init() override;

  virtual void on_start() override;

  virtual void on_update() override;

  virtual void on_render() override;

  virtual void on_release() override;

private:

  void record_frame(Rendering::CommandBuffer& rCmdBuffer);

  void update_parameters(Rendering::CommandBuffer& rCmdBuffer);

  void create_render_pass();

  void create_material();

  void init_constant_buffer();

  void initialize_param_set();

  void create_mesh();

  void update_assets(Rendering::CommandBuffer& rCmdBuffer);

  void draw_geometry(Rendering::CommandBuffer& rCmdBuffer);

  void submit_work(Rendering::CommandBuffer& rCmdBuffer);

  Rendering::BlendState m_blendState;
  Rendering::RasterState m_rasterState;
  Rendering::MultiSampleState m_multiSampleState;

  Rendering::RenderPass m_renderPass;
  Framework::Mesh m_mesh;
  Rendering::Material m_material;
  Rendering::ConstantBuffer m_testCB;
  Rendering::MaterialParameterSet m_parameterSet;
  Rendering::ParameterBufferGroup m_paramBufferGroup;
  Rendering::ParameterBuffer m_parameterBuffer;
  Rendering::RenderSurface m_renderSurface;
  Rendering::Buffer* m_pStagingBuffer;
  Rendering::Buffer* m_pUniformBuffer;

  Memory::MallocAllocator m_mallocAllocator;
  Memory::StackAllocator m_resStackAllocator; // Buffer used to allocate resources
  Memory::StackAllocator m_tmpStackAllocator; // Ring buffer for temporal allocations (no wrap)

  Rendering::FrameResourceFactory m_frameResourceFactory;

  TestCB m_testCBCPU;
  bool m_meshDirty;
};