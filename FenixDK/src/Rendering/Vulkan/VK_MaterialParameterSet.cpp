#include "VK_MaterialParameterSet.h"

#include "..\RenderInterface.h"
#include <unordered_map>
#include <algorithm>

namespace fdk
{
namespace Rendering
{
	namespace Vulkan
	{

    static std::vector<VkDescriptorSetLayoutBinding> g_layoutBindingConfig;

    // Table that will be access using EShaderParameterType value as an index.
    static const VkDescriptorType g_paramTypeConverterTable [] = 
    {
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
    };

    static const std::unordered_map<u32, VkShaderStageFlagBits> g_vulkanStageAccessFlagsMap
    {
      {Vertex, VK_SHADER_STAGE_VERTEX_BIT},
      { TesellationControl, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT },
      { TesellationEvaluation, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
      { Geometry, VK_SHADER_STAGE_GEOMETRY_BIT },
      { Fragment, VK_SHADER_STAGE_FRAGMENT_BIT },
      { Compute, VK_SHADER_STAGE_COMPUTE_BIT }
    };

    static inline VkDescriptorType shader_param_to_vk_type(const EShaderParameterType type) 
    {
      return g_paramTypeConverterTable[type];
    }

    static inline VkShaderStageFlagBits generate_vk_stage_mask(const u32 stageMask)
    {
      u32 vkMask = 0;
      for (u32 i=0; i<EShaderStageFlag::StageCount; i++) 
      {
        u32 mask = (1 << i);
        if (stageMask | mask) 
        {
          vkMask |= g_vulkanStageAccessFlagsMap.at(mask);
        }
      }
      return static_cast<VkShaderStageFlagBits>(vkMask);
    }

    VK_MaterialParameterSet::VK_MaterialParameterSet()
      : m_setLayout{VK_NULL_HANDLE,VK_NULL_HANDLE ,VK_NULL_HANDLE } {}

    void VK_MaterialParameterSet::create()
    {
      create_set_layout(Framerate_Scene);
      create_set_layout(Framerate_Material);
      create_set_layout(Framerate_Instance);
    }

    void VK_MaterialParameterSet::create_set_layout(const EParameterSetFramerateType framerate)
    {
      u32 count = std::count_if(m_parameters.begin(), m_parameters.end(), [&framerate](const ParameterDefinition& rParam) 
      {
        return rParam.m_setSlot == framerate; 
      });
      if (count != 0) 
      {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.pNext = nullptr;
        layoutInfo.flags = 0;
        layoutInfo.bindingCount = count;
        g_layoutBindingConfig.reserve(count);
        layoutInfo.pBindings = populateConfigurationsForVulkan(framerate);
        auto result = vkCreateDescriptorSetLayout(RenderInterface::s_device, &layoutInfo, nullptr, &m_setLayout[framerate]);
        VK_CHECK(result, "Failed to create the descriptor set layout");
      }
      else 
      {
        m_setLayout[framerate] = VK_NULL_HANDLE;
      }
    }

    VkDescriptorSetLayoutBinding* VK_MaterialParameterSet::populateConfigurationsForVulkan(const EParameterSetFramerateType framerate)
    {
      // TODO: Use a stack allocator to perform this logic. Use the vector as a global var is not secure.
      g_layoutBindingConfig.clear();
      for (auto& rParam : m_parameters) 
      {
        if (rParam.m_setSlot == framerate) 
        {
          VkDescriptorSetLayoutBinding binding;
          binding.binding = rParam.m_bindingSlot;
          binding.descriptorCount = 1; //TODO: Extend that to give support for arrays.
          binding.descriptorType = shader_param_to_vk_type(rParam.m_paramType);
          binding.pImmutableSamplers = nullptr; // TODO: Give support to initialize immutable samplers
          binding.stageFlags = generate_vk_stage_mask(rParam.m_stages);
          g_layoutBindingConfig.push_back(binding);
        }
      }
      if (g_layoutBindingConfig.size() > 0)
      {
        return &g_layoutBindingConfig[0];
      }
      return nullptr;
    }

  }
}
}
