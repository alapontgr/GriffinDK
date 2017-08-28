#pragma once

#include "Utilities/types.h" 
#include "Framework/app.h"
#include "Rendering/context.h"
#include "Framework/Time.h"
#include "Entry.h"


#include "Rendering/VK_RenderInterface.h"
#include "Framework/Mesh.h"
#include "Memory/MemAllocator.h"
#include "Memory/MallocAllocator.h"
#include "Rendering/Material.h"
#include "Rendering/FrameResourceFactory.h"
#include "Rendering/RenderSurface.h"


using namespace fdk;

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

  void create_render_pass();

  void create_material();

  void create_mesh();

  void update_assets(Rendering::CommandBuffer& rCmdBuffer);

  void draw_geometry(Rendering::CommandBuffer& rCmdBuffer);

  void submit_work(Rendering::CommandBuffer& rCmdBuffer);

  Rendering::BlendState m_blendState;
  Rendering::RasterState m_rasterState;
  Rendering::MultiSampleState m_multiSampleState;

  Rendering::VK_RenderInterface* m_pRi;

  Rendering::RenderPass m_renderPass;
  Framework::Mesh m_mesh;
  Rendering::Material* m_pMaterial;
  Rendering::Buffer* m_pStagingBuffer;
  Rendering::RenderSurface m_renderSurface;

  Memory::MallocAllocator m_mallocAllocator;

  Rendering::FrameResourceFactory m_frameResourceFactory;
  std::vector<VkCommandBuffer> m_cmdList;

  bool m_meshDirty;
};