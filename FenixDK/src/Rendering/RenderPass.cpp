#include "RenderPass.h"

#include "Utilities/platform.h"
#include "../Utilities/VulkanInclude.h"
#include "RenderDefines.h"

#include "Rendering/VK_RenderInterface.h"
#include "Rendering/VK_CommandBuffer.h"

namespace fdk {
namespace Rendering {

RenderPass::RenderPass() {}

RenderPass::~RenderPass() {}

void RenderPass::init(RenderInterface& rRI) {
  VK_RenderInterface* pRI = IMPLEMENTATION(RenderInterface, &rRI);

  // Description of the whole render pass
  VkAttachmentDescription attachmentsDesc{};
  attachmentsDesc.flags = 0;
  attachmentsDesc.format = pRI->m_swapChainFormat.format;
  attachmentsDesc.samples = VK_SAMPLE_COUNT_1_BIT;
  attachmentsDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachmentsDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachmentsDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachmentsDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachmentsDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // The first frame the layout is not VK_IMAGE_LAYOUT_PRESENT_SRC_KHR. So using Undefined solves the problem with the validation layer
  attachmentsDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  // Reference for the color attachment of the sub pass 0
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
  renderPassInfo.pAttachments = &attachmentsDesc;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subPassDesc;
  renderPassInfo.dependencyCount = static_cast<u32>(dependencies.size());
  renderPassInfo.pDependencies = &dependencies[0];

  auto result = vkCreateRenderPass(pRI->m_device, &renderPassInfo, nullptr,
                                   &m_renderPassImpl);
  VK_CHECK(result, "Failed to create render pass");
}

void RenderPass::release(RenderInterface& rRI)
{
  VK_RenderInterface* pRI = IMPLEMENTATION(RenderInterface, &rRI);

  if (m_renderPassImpl) 
  {
    vkDestroyRenderPass(pRI->m_device, m_renderPassImpl, nullptr);
  }
  m_renderPassImpl = VK_NULL_HANDLE;
}

void RenderPass::start(RenderInterface& rRI, CommandBuffer& rCmdBuffer)
{
  VK_RenderInterface* pRI = IMPLEMENTATION(RenderInterface, &rRI);
  VK_CommandBuffer* pCmdBuff = IMPLEMENTATION(CommandBuffer, &rCmdBuffer);
  
  // Begin render pass
//   u32 width = Framework::App::width();
//   u32 height = Framework::App::height();
//   VkRenderPassBeginInfo passBeginInfo{};
//   passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//   passBeginInfo.pNext = nullptr;
//   passBeginInfo.renderPass = m_renderPass.handle();
//   passBeginInfo.framebuffer = frame.frameBuffer;
//   passBeginInfo.renderArea.extent.width = width;
//   passBeginInfo.renderArea.extent.height = height;
//   passBeginInfo.renderArea.offset.x = 0;
//   passBeginInfo.renderArea.offset.y = 0;
//   passBeginInfo.clearValueCount = 1;
//   passBeginInfo.pClearValues = &clearColor;
//   vkCmdBeginRenderPass(frame.cmdBuffer, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::next_subpass(RenderInterface& rRI, CommandBuffer& rCmdBuffer)
{
  VK_RenderInterface* pRI = IMPLEMENTATION(RenderInterface, &rRI);
  VK_CommandBuffer* pCmdBuff = IMPLEMENTATION(CommandBuffer, &rCmdBuffer);

  //vkCmdNextSubpass(pCmdBuff->m_commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::end(RenderInterface& rRI, CommandBuffer& rCmdBuffer)
{
  VK_RenderInterface* pRI = IMPLEMENTATION(RenderInterface, &rRI);
  VK_CommandBuffer* pCmdBuff = IMPLEMENTATION(CommandBuffer, &rCmdBuffer);
  //vkCmdEndRenderPass(pCmdBuff->m_commandBuffer);
}

}
}
