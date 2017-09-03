#include "VK_Material.h"
#include "Utilities\platform.h"
#include "IO\FileUtils.h"
#include "../RenderInterface.h"

namespace fdk
{
  namespace Rendering 
  {
    
    struct VertexDesc // Temporary
    {
      v4 position;
      v4 colour;
    };

    VK_Material::VK_Material()
      : m_pipeline{VK_NULL_HANDLE}
    {

    }

    void VK_Material::init(const MaterialDesc& rDesc)
    {
      m_desc = rDesc;
    }

    void VK_Material::create_material(RenderInterface& rRI)
    {

      // Load shader modules
      VkShaderModule vsMmodule = create_shader_module(rRI.m_device, m_desc.m_vsPath.c_str());
      VkShaderModule psMmodule = create_shader_module(rRI.m_device, m_desc.m_psPath.c_str());

      // Define stages
      static constexpr u32 kStageCount = 2;
      static VkPipelineShaderStageCreateInfo kStageInfo[kStageCount];
      define_stage_info(VK_SHADER_STAGE_VERTEX_BIT, vsMmodule, "main", &kStageInfo[0]);
      define_stage_info(VK_SHADER_STAGE_FRAGMENT_BIT, psMmodule, "main", &kStageInfo[1]);


      // Parse input assembler format
      std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions(1);
      vertexBindingDescriptions[0].binding = 0;
      vertexBindingDescriptions[0].stride = sizeof(VertexDesc);
      vertexBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

      std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions(2); // One for each parameter in the input layout
      vertexAttributeDescriptions[0].location = 0;
      vertexAttributeDescriptions[0].binding = vertexBindingDescriptions[0].binding;
      vertexAttributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
      vertexAttributeDescriptions[0].offset = 0;
      vertexAttributeDescriptions[1].location = 1;
      vertexAttributeDescriptions[1].binding = vertexBindingDescriptions[0].binding;
      vertexAttributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
      vertexAttributeDescriptions[1].offset = offsetof(struct VertexDesc, colour);

      VkPipelineVertexInputStateCreateInfo vertexShaderInfo{};
      vertexShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
      vertexShaderInfo.pNext = nullptr;
      vertexShaderInfo.flags = 0;
      vertexShaderInfo.vertexBindingDescriptionCount = static_cast<u32>(vertexBindingDescriptions.size());
      vertexShaderInfo.pVertexBindingDescriptions = &vertexBindingDescriptions[0];
      vertexShaderInfo.vertexAttributeDescriptionCount = static_cast<u32>(vertexAttributeDescriptions.size());
      vertexShaderInfo.pVertexAttributeDescriptions = &vertexAttributeDescriptions[0];

      // Define topology to feed the shaders
      VkPipelineInputAssemblyStateCreateInfo assemblerInfo;
      assemblerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
      assemblerInfo.pNext = nullptr;
      assemblerInfo.flags = 0;
      assemblerInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
      assemblerInfo.primitiveRestartEnable = VK_FALSE;

      // Define default viewport and scissors
      VkViewport viewport;
      viewport.x = 0.0f; // Left
      viewport.y = 0.0f; // Top
      viewport.width = 512.0f;
      viewport.height = 512.0f;
      viewport.minDepth = 0.0f;
      viewport.maxDepth = 1.0f;
      VkRect2D scissors;
      scissors.offset.x = 0;
      scissors.offset.y = 0;
      scissors.extent.width = 512;
      scissors.extent.height = 512;


      VkPipelineViewportStateCreateInfo viewportStateInfo;
      viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
      viewportStateInfo.pNext = nullptr;
      viewportStateInfo.flags = 0;
      viewportStateInfo.viewportCount = 1;
      viewportStateInfo.pViewports = nullptr; //&viewport;
      viewportStateInfo.scissorCount = 1;
      viewportStateInfo.pScissors = nullptr; //&scissors;

                                             // Configure rasterizer
      VkPipelineRasterizationStateCreateInfo rasterInfo{};
      rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
      rasterInfo.pNext = nullptr;
      rasterInfo.flags = 0;
      rasterInfo.depthClampEnable = VK_FALSE;
      rasterInfo.rasterizerDiscardEnable = VK_FALSE;
      //If we want to use different modes we have to enable the corresponding feature
      rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
      rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
      rasterInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
      rasterInfo.depthBiasEnable = VK_FALSE;
      rasterInfo.depthBiasConstantFactor = 0.0f;
      rasterInfo.depthBiasClamp = 0.0f;
      rasterInfo.depthBiasSlopeFactor = 0.0f;
      rasterInfo.lineWidth = 1.0f;

      // Configure multi sampling state
      VkPipelineMultisampleStateCreateInfo multiSamplingInfo{};
      multiSamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
      multiSamplingInfo.pNext = nullptr;
      multiSamplingInfo.flags = 0;
      multiSamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
      multiSamplingInfo.sampleShadingEnable = VK_FALSE;
      multiSamplingInfo.minSampleShading = 1.0f;
      multiSamplingInfo.pSampleMask = nullptr;
      multiSamplingInfo.alphaToCoverageEnable = VK_FALSE;
      multiSamplingInfo.alphaToOneEnable = VK_FALSE;

      // Configure the Blend state
      VkPipelineColorBlendAttachmentState colorBlendState{};
      colorBlendState.blendEnable = VK_FALSE;
      colorBlendState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
      colorBlendState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
      colorBlendState.colorBlendOp = VK_BLEND_OP_ADD;
      colorBlendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      colorBlendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
      colorBlendState.alphaBlendOp = VK_BLEND_OP_ADD;
      colorBlendState.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;

      VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
      colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
      colorBlendInfo.pNext = nullptr;
      colorBlendInfo.flags = 0;
      colorBlendInfo.logicOpEnable = VK_FALSE;
      colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
      colorBlendInfo.attachmentCount = 1;
      colorBlendInfo.pAttachments = &colorBlendState;
      colorBlendInfo.blendConstants[0] = 0.0f;
      colorBlendInfo.blendConstants[1] = 0.0f;
      colorBlendInfo.blendConstants[2] = 0.0f;
      colorBlendInfo.blendConstants[3] = 0.0f;

      // Define dynamic properties
      std::vector<VkDynamicState> dynamicStates{
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR };
      VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
      dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
      dynamicStateInfo.pNext = nullptr;
      dynamicStateInfo.flags = 0;
      dynamicStateInfo.dynamicStateCount = static_cast<u32>(dynamicStates.size());
      dynamicStateInfo.pDynamicStates = &dynamicStates[0];

      create_layout(rRI.m_device);

      // Create the graphic pipeline
      VkGraphicsPipelineCreateInfo pipelineInfo{};
      pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
      pipelineInfo.pNext = nullptr;
      pipelineInfo.flags = 0;
      pipelineInfo.stageCount = static_cast<u32>(kStageCount);
      pipelineInfo.pStages = kStageInfo;
      pipelineInfo.pVertexInputState = &vertexShaderInfo;
      pipelineInfo.pInputAssemblyState = &assemblerInfo;
      pipelineInfo.pTessellationState = nullptr;
      pipelineInfo.pViewportState = &viewportStateInfo;
      pipelineInfo.pRasterizationState = &rasterInfo;
      pipelineInfo.pMultisampleState = &multiSamplingInfo;
      pipelineInfo.pDepthStencilState = nullptr;
      pipelineInfo.pColorBlendState = &colorBlendInfo;
      pipelineInfo.pDynamicState = &dynamicStateInfo;
      pipelineInfo.layout = m_layout;
      pipelineInfo.renderPass = m_desc.m_pRenderPass->handle();
      pipelineInfo.subpass = 0;
      pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
      pipelineInfo.basePipelineIndex = -1;

      auto result = vkCreateGraphicsPipelines(rRI.m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline);
      VK_CHECK(result, "Failed to create Graphics Pipeline");

      // Destroy modules now that are not needed
      destroy_shader_module(rRI.m_device, vsMmodule);
      destroy_shader_module(rRI.m_device, psMmodule);

    }

    EResultType VK_Material::create_layout(VkDevice pDevice)
    {
      // Create an empty layout without descriptors (No parameters)
      // TODO Add parameters
      VkPipelineLayoutCreateInfo layoutInfo{};
      layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
      layoutInfo.pNext = nullptr;
      layoutInfo.flags = 0;
      layoutInfo.setLayoutCount = 0;
      layoutInfo.pSetLayouts = nullptr;
      layoutInfo.pushConstantRangeCount = 0;
      layoutInfo.pPushConstantRanges = nullptr;
      auto result = vkCreatePipelineLayout(pDevice, &layoutInfo, nullptr, &m_layout);
      VK_CHECK(result, "Failed to create Pipeline Layout");
      return kNoError;
    }

    VkShaderModule VK_Material::create_shader_module(VkDevice pDevice, const char* pFilePath)
    {
      IO::File source;
      source.load(pFilePath, 0, alignof(u32));
      FDASSERT(source.size() > 0, "Loaded empty file");

      VkShaderModuleCreateInfo info;
      info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
      info.pNext = nullptr;
      info.flags = 0;
      info.codeSize = source.size();
      info.pCode = reinterpret_cast<u32*>(source.data());

      VkShaderModule module = VK_NULL_HANDLE;
      auto result = vkCreateShaderModule(pDevice, &info, nullptr, &module);
      VK_CHECK(result, "Failed to create shader");

      source.release();

      return module;
    }

    void VK_Material::destroy_shader_module(VkDevice pDevice, VkShaderModule pShaderModule)
    {
      FDASSERT(pShaderModule != VK_NULL_HANDLE, "Invalid shader module");
      vkDestroyShaderModule(pDevice, pShaderModule, nullptr);
    }


    void VK_Material::define_stage_info(
      VkShaderStageFlagBits stageFlags, 
      VkShaderModule pShaderModule, 
      const char* pEntryName, 
      VkPipelineShaderStageCreateInfo* pStageInfoOut)
    {
      pStageInfoOut->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
      pStageInfoOut->pNext = nullptr;
      pStageInfoOut->flags = 0;
      pStageInfoOut->stage = stageFlags;
      pStageInfoOut->module = pShaderModule;
      pStageInfoOut->pName = pEntryName;
      pStageInfoOut->pSpecializationInfo = nullptr;
    }

  }
}

