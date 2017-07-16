#pragma once
#include "Material.h"

#include "Utilities/VulkanInclude.h"
#include "Utilities/GeneralDefinitions.h"
#include "RenderConfiguration.h"

namespace fdk
{
  namespace Rendering 
  {
    class VK_Material : public Material
    {
    public:

      EResultType create(VkDevice pDevice);

      VkPipelineLayout m_layout;
      VkPipeline m_pipeline;
   
      EResultType create_layout(VkDevice pDevice);

      VkShaderModule create_shader_module(VkDevice pDevice, const char* pFilePath);

      void destroy_shader_module(VkDevice pDevice, VkShaderModule pShaderModule);

      VkViewport get_viewport_desc(const Viewport& rViewport);

      VkRect2D get_scissors_desc(const Scissor& rScissor);

      VkPipelineRasterizationStateCreateInfo get_raster_state_desc(const RasterState& rRasterState);

      VkPipelineColorBlendAttachmentState configure_blend_attachment_state(const BlendState& rBlendState);

      VkPipelineColorBlendStateCreateInfo get_blend_state_desc(const BlendState& rBlendState, VkPipelineColorBlendAttachmentState* pBlendAttachmentState);
    
      void define_stage_info(VkShaderStageFlagBits stageFlags, VkShaderModule pShaderModule, const char* pEntryName, VkPipelineShaderStageCreateInfo* pStageInfoOut);
    };

  }
}