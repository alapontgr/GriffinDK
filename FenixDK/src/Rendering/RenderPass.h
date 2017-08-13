#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk {
namespace Rendering {

class RenderInterface;

class RenderPass {

public:
  RenderPass();
  ~RenderPass();

  void init(RenderInterface& rRI);

  void release(RenderInterface& rRI);

  VkRenderPass handle() const { return m_renderPassImpl; }

private:
  VkRenderPass m_renderPassImpl;
};

}
}