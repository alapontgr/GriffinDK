#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk {
namespace Rendering {

class Context;
class RenderPass;

class Framebuffer {
public:
  Framebuffer();
  ~Framebuffer();

  void init(const Context& rContext, const RenderPass& rRenderPass);

  void release(const Context& context);

  VkFramebuffer get() { return m_handle; }

  VkImageView m_imageView;
private:
  VkFramebuffer m_handle;
};

}
}