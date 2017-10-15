#pragma once

#include "..\MaterialParameterSetBase.h"

namespace fdk
{
  namespace Rendering 
  {
    namespace Vulkan 
    {
      class VK_MaterialParameterSet : public MaterialParameterSetBase
      {
      public:

        VK_MaterialParameterSet();

        void create();

        VkDescriptorSetLayout m_pLayout;

      private:

        VkDescriptorSetLayoutBinding* populateConfigurationsForVulkan();

      };

    }
  }
}