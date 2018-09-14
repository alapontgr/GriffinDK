#pragma once

#include "../ParameterBufferGroupBase.h"

#include "Utilities/VulkanInclude.h"

namespace fdk
{
  namespace Rendering 
  {
    namespace Vulkan 
    {
      class VK_ParameterBufferGroup : public ParameterBufferGroupBase
      {
      public:

        VK_ParameterBufferGroup();

        void create();

        VkDescriptorPool m_descriptorPool;
      };

    }
  }
}