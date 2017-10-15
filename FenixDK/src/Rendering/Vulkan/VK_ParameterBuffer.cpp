#include "VK_ParameterBuffer.h"

#include "..\RenderInterface.h"
#include "..\MaterialParameterSet.h"

namespace fdk
{
  namespace Rendering 
  {
    namespace Vulkan 
    {
      
      VK_ParameterBuffer::VK_ParameterBuffer()
      {

      }

      void VK_ParameterBuffer::create()
      {
        /*VkDescriptorSetAllocateInfo descSetAllocInfo{};
        descSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descSetAllocInfo.pNext = nullptr;
        descSetAllocInfo.descriptorPool = RenderInterface::s_po;
        descSetAllocInfo.descriptorSetCount = 1;
        descSetAllocInfo.pSetLayouts = &m_fragUniformLayout;
        auto result = vkAllocateDescriptorSets(m_device, &descSetAllocInfo, &m_fragDescSet);
        VK_CHECK(result, "Failed to allocate the descriptor set");
*/
      }

      void VK_ParameterBuffer::update()
      {

      }

      void VK_ParameterBuffer::release()
      {

      }

      void VK_ParameterBuffer::bind()
      {

      }

    }
  }
}

