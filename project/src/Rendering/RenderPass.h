#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk {
namespace Rendering {

class Context;

class RenderPass {

public:
  RenderPass();
  ~RenderPass();

  void init(const Context& rContext);

  void release(const Context& context);

  VkRenderPass handle() const { return m_renderPassImpl; }

private:
  VkRenderPass m_renderPassImpl;
};

}
}