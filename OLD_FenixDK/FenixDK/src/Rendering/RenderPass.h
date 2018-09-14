#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk {
namespace Rendering {

class CommandBuffer;
class Framebuffer;

// TODO: Refactor that. Hide VK implementation of a render pass.
class RenderPass {

public:
  RenderPass();
  ~RenderPass();

  void init();

  void release();

  VkRenderPass handle() const { return m_renderPassImpl; }

  void start(CommandBuffer& rCmdBuffer, Framebuffer& rFramebuffer);

  void next_subpass(CommandBuffer& rCmdBuffer);

  void end(CommandBuffer& rCmdBuffer);

private:

  VkRenderPass m_renderPassImpl;
};

}
}