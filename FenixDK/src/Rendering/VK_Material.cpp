#include "VK_Material.h"
#include "Utilities\platform.h"
#include "IO\FileUtils.h"

namespace fdk
{
  namespace Rendering 
  {
    
    EResultType VK_Material::create(VkDevice pDevice)
    {
      // ...
      // TODO Create shader modules
      // TODO Parse parameters
      // TODO Parse input assembler format

      // TODO Define stages

      create_layout(pDevice);

      // ...
      return kNoError;
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
      FDK_ASSERT(source.size() > 0, "Loaded empty file");

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

  }
}

