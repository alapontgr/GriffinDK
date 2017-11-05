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

        VkDescriptorSetLayout m_setLayout[Framerate_Count];

      private:

        void create_set_layout(const EParameterSetFramerateType framerate);

        VkDescriptorSetLayoutBinding* populateConfigurationsForVulkan(const EParameterSetFramerateType framerate);

      };

    }
  }
}