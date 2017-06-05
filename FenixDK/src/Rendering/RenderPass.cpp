#include "RenderPass.h"

#include "context.h"

#include "../Utilities/VulkanInclude.h"
#include "Utilities/platform.h"

namespace fdk {
namespace Rendering {

RenderPass::RenderPass() {}

RenderPass::~RenderPass() {}

void RenderPass::init(const Context &context) {
  // Description of the whole render pass
  VkAttachmentDescription renderPassDesc{};
  renderPassDesc.flags = 0;
  renderPassDesc.format = context.swap_chain_images_format();
  renderPassDesc.samples = VK_SAMPLE_COUNT_1_BIT;
  renderPassDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  renderPassDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  renderPassDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  renderPassDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  renderPassDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // The first frame the layout is not VK_IMAGE_LAYOUT_PRESENT_SRC_KHR. So using Undefined solves the problem with the validation layer
  renderPassDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  // Reference for the color attachment of the subpass 0
  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // Description of the sub pass 0
  VkSubpassDescription subPassDesc{};
  subPassDesc.flags = 0;
  subPassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subPassDesc.inputAttachmentCount = 0;
  subPassDesc.pInputAttachments = nullptr;
  subPassDesc.colorAttachmentCount = 1;
  subPassDesc.pColorAttachments = &colorAttachmentRef;
  subPassDesc.pResolveAttachments = nullptr;
  subPassDesc.pDepthStencilAttachment = nullptr;
  subPassDesc.preserveAttachmentCount = 0;
  subPassDesc.pPreserveAttachments = nullptr;

  // Configure dependencies
  static constexpr u32 kDependencyCount = 2;
  std::vector<VkSubpassDependency> dependencies(kDependencyCount);
  dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  dependencies[0].dstSubpass = 0; // Sub pass index
  dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  dependencies[1].srcSubpass = 0; // Sub pass index
  dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
  dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


  // Create the render pass
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.pNext = nullptr;
  renderPassInfo.flags = 0;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &renderPassDesc;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subPassDesc;
  renderPassInfo.dependencyCount = static_cast<u32>(dependencies.size());
  renderPassInfo.pDependencies = &dependencies[0];

  auto result = vkCreateRenderPass(context.device(), &renderPassInfo, nullptr,
                                   &m_renderPassImpl);
  VK_CHECK(result, "Failed to create render pass");
}

void RenderPass::release(const Context& context)
{
  if (m_renderPassImpl) 
  {
    vkDestroyRenderPass(context.device(), m_renderPassImpl, nullptr);
  }
  m_renderPassImpl = VK_NULL_HANDLE;
}

}
}
