#include "VK_ParameterBufferGroup.h"
#include "..\RenderInterface.h"

namespace fdk
{
namespace Rendering
{
	namespace Vulkan
	{
    // TODO: This is UGLY. Refactor this class
    static constexpr u32 kPerTypeMaxAllocations = 128;
    static constexpr u32 kMaxDescriptorSetAllocations = 64;
    static constexpr u32 kTypeCount = 2;
    VkDescriptorPoolSize g_defaultAllocationPreset[] = 
    {
      { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, kPerTypeMaxAllocations },
      { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, kPerTypeMaxAllocations }
    };

		VK_ParameterBufferGroup::VK_ParameterBufferGroup()
      : m_descriptorPool{VK_NULL_HANDLE}
		{
		}

		void VK_ParameterBufferGroup::create()
		{
      VkDescriptorPoolCreateInfo descPoolInfo{};
      descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
      descPoolInfo.pNext = nullptr;
      descPoolInfo.flags = 0;
      descPoolInfo.poolSizeCount = kTypeCount;
      descPoolInfo.pPoolSizes = g_defaultAllocationPreset;
      descPoolInfo.maxSets = kMaxDescriptorSetAllocations;
      auto result = vkCreateDescriptorPool(RenderInterface::s_device, &descPoolInfo, nullptr, &m_descriptorPool);
      VK_CHECK(result, "Failed to create the descriptor pool");
		}
	}
}
}
