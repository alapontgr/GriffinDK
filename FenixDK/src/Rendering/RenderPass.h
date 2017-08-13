#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk {
namespace Rendering {

class RenderInterface;
class CommandBuffer;

class RenderPass {

public:
  RenderPass();
  ~RenderPass();

  void init(RenderInterface& rRI);

  void release(RenderInterface& rRI);

  VkRenderPass handle() const { return m_renderPassImpl; }

  void start(RenderInterface& rRI, CommandBuffer& rCmdBuffer);

  void next_subpass(RenderInterface& rRI, CommandBuffer& rCmdBuffer);

  void end(RenderInterface& rRI, CommandBuffer& rCmdBuffer);

private:
  VkRenderPass m_renderPassImpl;
};

}
}