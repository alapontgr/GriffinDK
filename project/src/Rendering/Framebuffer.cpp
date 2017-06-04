#include "Framebuffer.h"

#include "context.h"
#include "RenderPass.h"

#include "Framework\app.h"
#include "Utilities\platform.h"

namespace dk {
namespace Rendering {

Framebuffer::Framebuffer() {}

Framebuffer::~Framebuffer() {}

void Framebuffer::init(const Context& rContext, const RenderPass& rRenderPass)
{
  VkFramebufferCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  info.pNext = nullptr;
  info.flags = 0;
  info.renderPass = rRenderPass.handle();
  info.attachmentCount = 1;
  info.pAttachments = &m_imageView;
  info.width = Framework::App::width();
  info.height = Framework::App::height();
  info.layers = 1;
  auto result = vkCreateFramebuffer(rContext.device(), &info, nullptr, &m_handle);
  VK_CHECK(result, "Failed to create framebuffer");
}

void Framebuffer::release(const Context& context)
{
  if (m_imageView)
  {
    vkDestroyImageView(context.device(), m_imageView, nullptr);
    m_imageView = VK_NULL_HANDLE;
  }
  if (m_handle) 
  {
    vkDestroyFramebuffer(context.device(), m_handle, nullptr);
    m_handle = VK_NULL_HANDLE;
  }
}

}
}
